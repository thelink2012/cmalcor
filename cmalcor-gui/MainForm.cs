using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Cyotek.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters.Binary;
using System.Runtime.Serialization;
using Microsoft.Win32;
using static System.Environment;

namespace CmAlcorGUI
{
    public partial class MainForm : DarkUI.Forms.DarkForm
    {
        public delegate Color ColorPictureBoxCb(Color c, int ch);

        /// <summary>
        /// Directory for storing app settings and the startup dll.
        /// </summary>
        string alcorData = Path.Combine(GetFolderPath(SpecialFolder.CommonApplicationData), "CmAlcor");

        /// <summary>
        /// Whether the colors pallete is being updated at the current moment.
        /// </summary>
        private bool updatingPallete = false;

        /// <summary>
        /// Whether the mouse memory is being programmed at the current moment.
        /// </summary>
        private bool programmingMouse = false;

        /// <summary>
        /// The current color in the HSL sliders.
        /// </summary>
        public HslColor ColorSlider
        {
            get
            {
                return new HslColor(hueColorSlider.Value,
                                    saturationColorSlider.Value / 100.0,
                                    lightnessColorSlider.Value / 100.0);
            }

            set
            {
                hueColorSlider.Value = (float) value.H;
                saturationColorSlider.Value = (float) value.S * 100.0f;
                lightnessColorSlider.Value = (float) value.L * 100.0f;
            }
        }

        /// <summary>
        /// Creates the form components.
        /// </summary>
        public MainForm()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Initialises the form.
        /// </summary>
        private void MainForm_Load(object sender, EventArgs e)
        {
            // Load user settings before anything.
            LoadSettings();
            
            // Default colors, taken from the Firmware Table. Used on CM Mizar too.
            this.colorGrid1.Colors = new ColorCollection(new Color[] {
                //Color.FromArgb(0, 0, 0),
                Color.FromArgb(0xFF, 0, 0),
                Color.FromArgb(0, 0xFF, 0),
                Color.FromArgb(0, 0, 0xFF),
                Color.FromArgb(0x9E, 0xA0, 0),
                //Color.FromArgb(0xA0, 0, 0x4E),
                Color.FromArgb(0, 0x4E, 0xA0),
                Color.FromArgb(0xFF, 0x40, 0),
                Color.FromArgb(0x52, 0, 0xA0),
                Color.FromArgb(0xFF, 0xDA, 0xAA),
            });
            
            this.startupHandlerBtn.Enabled = !HasStartupHandlerInstalled();
            
            UpdateUIFromMouseMemory();
        }
        
        /// <summary>
        /// 
        /// </summary>
        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if(programmingMouse)
            {
                // Bad things can happen if we stop the mouse programming.
                e.Cancel = true;
            }
            else
            {
                SaveSettings();
            }
        }

        /// <summary>
        /// 
        /// </summary>
        private void UpdateUIFromMouseMemory()
        {
            // default values
            var currMode = Firmware.LedMode.AlwaysOn;
            int currBrightness = 10;
            byte currRed = 0xFF, currGreen = 0x00, currBlue = 0x00;

            try
            {
                Firmware.GetLED(ref currMode, ref currBrightness, ref currRed, ref currGreen, ref currBlue);
            }
            catch(FirmwareException ex)
            {
                MessageBox.Show(ex.Message, "CmAlcorGUI", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            finally
            {
                this.ColorSlider = Color.FromArgb(currRed, currGreen, currBlue);
                ledBrightnessSlider.Value = currBrightness;
                SetRadioFromLedMode(currMode);
            }

            UpdateLedImageNow();
        }



        #region Settings

        /// <summary>
        /// Loads app settings from last session.
        /// </summary>
        private void LoadSettings()
        {
            string path = Path.Combine(alcorData, "CmAlcorGUI.config.bin");

            try
            {
                this.updatingPallete = true;
			    using (Stream stream = File.Open(path, FileMode.Open))
			    {
			        BinaryFormatter fmt = new BinaryFormatter();
			        var appSettings = (AppSettings) fmt.Deserialize(stream);
                    colorGrid1.CustomColors = new ColorCollection(appSettings.customColors);
			    }
            }
            catch(IOException) { /* most likely file doesn't exist */ }
            catch(SerializationException) { /* ignore anyway */ }
            finally
            {
                this.updatingPallete = false;
            }
        }

        /// <summary>
        /// Saves app settings from this session.
        /// </summary>
        private void SaveSettings()
        {
            string path = Path.Combine(alcorData, "CmAlcorGUI.config.bin");
            
            try
            {
                // avoid saving settings if no user color has been set
                if(!colorGrid1.CustomColors.All(c => c == Color.White || c == Color.Black))
                {
                    Directory.CreateDirectory(alcorData);
                
                    var appSettings = new AppSettings();
                    appSettings.customColors = colorGrid1.CustomColors.ToList();

			        using (Stream stream = File.Open(path, FileMode.Create))
			        {
			            BinaryFormatter fmt = new BinaryFormatter();
			            fmt.Serialize(stream, appSettings);
			        }
                }
            }
            catch(IOException)
            {
                // Ignore
            }
        }

        #endregion

        
        #region Startup Handler

        /// <summary>
        /// Checks whether the startup library at %ProgramData% is older than the one in the directory of this GUI app.
        /// </summary>
        private bool IsStartupLibraryOlderThanCurrent()
        {
            bool isOlder = true;

            IntPtr libCmAlcor = NativeMethods.LoadLibrary(Path.Combine(alcorData, "cmalcor.dll"));
            if(libCmAlcor != IntPtr.Zero) // when doesn't exist or any other error
            {
                IntPtr ptrLibraryVersion = NativeMethods.GetProcAddress(libCmAlcor, "CmAlcor_LibraryVersion");
                if(ptrLibraryVersion != IntPtr.Zero)
                {
                    var programDataLibraryVersion = (Firmware.Delegate_CmAlcor_LibraryVersion) Marshal.GetDelegateForFunctionPointer(ptrLibraryVersion, typeof(Firmware.Delegate_CmAlcor_LibraryVersion));
                    isOlder = (programDataLibraryVersion() < Firmware.LibraryVersion());
                }
                NativeMethods.FreeLibrary(libCmAlcor);
            }
            return isOlder;
        }

        /// <summary>
        /// Checks whether the startup library is programmed to run on Windows startup.
        /// </summary>
        private bool HasStartupHandlerInstalled()
        {
            using(RegistryKey key = Registry.CurrentUser.OpenSubKey("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"))
            {
                return key.GetValue("CmAlcorLED") != null;
            }
        }

        /// <summary>
        /// Make startup library run on Windows startup.
        /// Note: This does not copy the startup library from GUI directory to ProgramData.
        /// </summary>
        private void AddStartupCommandToRegistry()
        {
            using(RegistryKey key = Registry.CurrentUser.OpenSubKey("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", true))
            {
                string command = String.Format("\"{0}\" {1},{2}",
                                                Path.Combine(GetFolderPath(SpecialFolder.System), "rundll32.exe"),
                                                Path.Combine(alcorData, "cmalcor.dll"),
                                                "CmAlcor_EpEnableCustomLED");
                        
                key.SetValue("CmAlcorLED", command);
                startupHandlerBtn.Enabled = false;
            }
        }

        /// <summary>
        /// Stop the startup library from running on Windows startup.
        /// </summary>
        private void RemoveStartupCommandFromRegistry()
        {
            using(RegistryKey key = Registry.CurrentUser.OpenSubKey("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", true))
            {
                key.DeleteValue("CmAlcorLED", false);
                startupHandlerBtn.Enabled = true;
            }
        }

        #endregion
        


        #region Led Mode Radio

        /// <summary>
        /// 
        /// </summary>
        private Firmware.LedMode GetLedModeFromRadio()
        {
            if(alwaysOffRadioBtn.Checked) return Firmware.LedMode.AlwaysOff;
            if(alwaysOnRadioBtn.Checked)  return Firmware.LedMode.AlwaysOn;
            if(rapidFireRadioBtn.Checked) return Firmware.LedMode.RapidFire;
            if(breathingRadioBtn.Checked) return Firmware.LedMode.Breathing;
            throw new InvalidOperationException();
        }

        /// <summary>
        /// 
        /// </summary>
        private void SetRadioFromLedMode(Firmware.LedMode mode)
        {
            switch(mode)
            {
                case Firmware.LedMode.AlwaysOff:
                    alwaysOffRadioBtn.Checked = true;
                    break;
                case Firmware.LedMode.AlwaysOn:
                    alwaysOnRadioBtn.Checked = true;
                    break;
                case Firmware.LedMode.RapidFire:
                    rapidFireRadioBtn.Checked = true;
                    break;
                case Firmware.LedMode.Breathing:
                    breathingRadioBtn.Checked = true;
                    break;
                default:
                    throw new InvalidOperationException();
            }
        }

        #endregion



        #region LED Image

        /// <summary>
        /// 
        /// </summary>
        private void UpdateLedImage()
        {
            timerUpdateLedImage.Stop();
            timerUpdateLedImage.Start();
        }

        /// <summary>
        /// 
        /// </summary>
        private void UpdateLedImageNow()
        {
            ColorizePictureBox(this.pictureBoxCm, (Color c, int ch) => {
                return new HslColor(c.A, hueColorSlider.Value, saturationColorSlider.Value / 100.0, lightnessColorSlider.Value / 100.0);
            });
        }
        
        /// <summary>
        /// Transforms the colors of a picture box acorrding to cbColorize.
        /// Taken from http://stackoverflow.com/a/28799612
        /// </summary>
        private static void ColorizePictureBox(PictureBox pictureBox, ColorPictureBoxCb cbColorize)
        {
            // we pull the bitmap from the image
            Bitmap bmp = (Bitmap)pictureBox.Image;
            Size s = bmp.Size;
            System.Drawing.Imaging.PixelFormat fmt = bmp.PixelFormat;

            // we need the bit depth and we assume either 32bppArgb or 24bppRgb !
            byte bpp = (byte)(fmt == System.Drawing.Imaging.PixelFormat.Format32bppArgb ? 4 : 3);

            // lock the bits and prepare the loop
            Rectangle rect = new Rectangle(Point.Empty, s);
            System.Drawing.Imaging.BitmapData bmpData = bmp.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadOnly, fmt);
            int size1 = bmpData.Stride * bmpData.Height;
            byte[] data = new byte[size1];
            System.Runtime.InteropServices.Marshal.Copy(bmpData.Scan0, data, 0, size1);

            // loops
            for (int y = 0; y < s.Height; y++)
            {
                for (int x = 0; x < s.Width; x++)
                {
                    // calculate the index
                    int index = y * bmpData.Stride + x * bpp;
                    // get the color
                    Color c = Color.FromArgb( bpp == 4 ?data[index + 3]: 255 , 
                                              data[index + 2], data[index + 1], data[index]);
                    // process it
                    c = cbColorize(c, 2); 

                    // set the channels from the new color
                    data[index + 0] = c.B;
                    data[index + 1] = c.G;
                    data[index + 2] = c.R;
                    if (bpp == 4) data[index + 3] = c.A;
                }
            }

            System.Runtime.InteropServices.Marshal.Copy(data, 0, bmpData.Scan0, data.Length);
            bmp.UnlockBits(bmpData);

            // we need to re-assign the changed bitmap
            pictureBox.Image = (Bitmap)bmp;
        }

        #endregion




        #region GUI Update Helpers

        /// <summary>
        /// Color on the saturation slider must be updated when the Hue and Lightness sliders change.
        /// </summary>
        private void UpdateSaturation()
        {
            var color = this.ColorSlider;
            this.saturationColorSlider.Color = new HslColor(color.H, 1.0, color.L);
        }
    
        /// <summary>
        /// Checks if pallete contains the current ColorSlider, if so, selects such color in the pallete.
        /// Otherwise, checks the last, placeholder, color.
        /// </summary>
        private void UpdatePalleteSelection()
        {
            var currentColor = this.ColorSlider.ToRgbColor();
            var colors = colorGrid1.Colors.Concat(colorGrid1.CustomColors).ToArray();

            var defaultIndex = colors.Length - 1;

            var pallIndex = colors.Select((color, index) => new { color, index })
                                  .Where(c => c.color == currentColor)
                                  .DefaultIfEmpty(new { color = currentColor, index = defaultIndex })
                                  .First().index;

            if(pallIndex == defaultIndex)
            {
                // Can't change the color of the last grid or things will fuck up.
                //
                // var newCustomColors = colorGrid1.CustomColors.ToArray();
                // newCustomColors[newCustomColors.Length - 1] = currentColor;
                // colorGrid1.CustomColors = new ColorCollection(newCustomColors);
                //
            }
            
            try
            {
                this.updatingPallete = true;
                colorGrid1.ColorIndex = pallIndex;
            }
            finally
            {
                this.updatingPallete = false;
            }
        }

        #endregion








        private void timerUpdateLedImage_Tick(object sender, EventArgs e)
        {
            UpdateLedImageNow();
            timerUpdateLedImage.Stop();
        }

        private void hueColorSlider_ValueChanged(object sender, EventArgs e)
        {
            UpdateSaturation();
            UpdateLedImage();
            if(!updatingPallete) UpdatePalleteSelection();
        }

        private void saturationColorSlider_ValueChanged(object sender, EventArgs e)
        {
            UpdateLedImage();
            if(!updatingPallete) UpdatePalleteSelection();
        }

        private void lightnessColorSlider_ValueChanged(object sender, EventArgs e)
        {
            UpdateSaturation();
            UpdateLedImage();
            if(!updatingPallete) UpdatePalleteSelection();
        }
        
        private void colorGrid1_ColorChanged(object sender, EventArgs e)
        {
            if(!updatingPallete)
            {
                if(colorGrid1.Color != Color.White)
                {
                    try
                    {
                        this.updatingPallete = true;
                        this.ColorSlider = colorGrid1.Color;
                    }
                    finally
                    {
                        this.updatingPallete = false;
                    }
                }
            }
        }
        
        private async void applyBtn_Click(object sender, EventArgs e)
        {
            bool startupBtnState = startupHandlerBtn.Enabled;

            try
            {
                this.programmingMouse = true;
                
                applyBtn.Enabled = false;
                defaultBtn.Enabled = false;
                startupHandlerBtn.Enabled = false;

                Color color = ColorSlider.ToRgbColor();
                await Firmware.SetLEDAsync(GetLedModeFromRadio(), ledBrightnessSlider.Value, color.R, color.G, color.B);

                Firmware.EnableCustomLED();

            }
            catch(FirmwareException ex)
            {
                // Mouse may be in a "waiting for write operations" state, better reset the mouse.
                // Additionally, scroll wheel and LED are most likely turned off, so user should indeed reconnect the mice.
                MessageBox.Show("Failed to apply changes to the mouse.\n" +
                                "\nIt's highly recommended that you disconnect and reconnect your mice right now to avoid further problems.\n" +
                                "\nAdditional error information: " + ex.Message,
                                "CmAlcorGUI", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            finally
            {
                this.programmingMouse = false;

                applyBtn.Enabled = true;
                defaultBtn.Enabled = true;
                startupHandlerBtn.Enabled = startupBtnState;
            }
        }
        
        private void defaultBtn_Click(object sender, EventArgs e)
        {
            try
            {
                Firmware.DisableCustomLED();
                RemoveStartupCommandFromRegistry();
            }
            catch(FirmwareException ex)
            {
                MessageBox.Show("Failed to apply changes to the mouse.\n" +
                                "\nAdditional error information: " + ex.Message,
                                "CmAlcorGUI", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void startupHandlerBtn_Click(object sender, EventArgs e)
        {
            try
            {
                if(IsStartupLibraryOlderThanCurrent())
                {
                    Directory.CreateDirectory(alcorData);

                    File.Copy(Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "cmalcor.dll"),
                              Path.Combine(alcorData, "cmalcor.dll"),
                              true);
                }

                if(File.Exists(Path.Combine(alcorData, "cmalcor.dll")))
                {
                    AddStartupCommandToRegistry();
                }
                else
                {
                    MessageBox.Show("Failed to copy cmalcor.dll to %ProgramData%.", "CmAlcorGUI", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            catch(IOException ex)
            {
                MessageBox.Show(ex.Message, "CmAlcorGUI", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
    }
}

