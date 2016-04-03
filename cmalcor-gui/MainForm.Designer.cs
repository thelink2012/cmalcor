namespace CmAlcorGUI
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if(disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.alwaysOnRadioBtn = new DarkUI.Controls.DarkRadioButton();
            this.alwaysOffRadioBtn = new DarkUI.Controls.DarkRadioButton();
            this.breathingRadioBtn = new DarkUI.Controls.DarkRadioButton();
            this.rapidFireRadioBtn = new DarkUI.Controls.DarkRadioButton();
            this.applyBtn = new DarkUI.Controls.DarkButton();
            this.defaultBtn = new DarkUI.Controls.DarkButton();
            this.startupHandlerBtn = new DarkUI.Controls.DarkButton();
            this.lightnessColorSlider = new Cyotek.Windows.Forms.LightnessColorSlider();
            this.saturationColorSlider = new Cyotek.Windows.Forms.SaturationColorSlider();
            this.hueColorSlider = new Cyotek.Windows.Forms.HueColorSlider();
            this.colorGrid1 = new Cyotek.Windows.Forms.ColorGrid();
            this.darkSeparator1 = new DarkUI.Controls.DarkSeparator();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.darkLabel5 = new DarkUI.Controls.DarkLabel();
            this.darkLabel4 = new DarkUI.Controls.DarkLabel();
            this.ledBrightnessSlider = new System.Windows.Forms.TrackBar();
            this.darkLabel3 = new DarkUI.Controls.DarkLabel();
            this.panel3 = new System.Windows.Forms.Panel();
            this.darkLabel2 = new DarkUI.Controls.DarkLabel();
            this.panel4 = new System.Windows.Forms.Panel();
            this.darkLabel1 = new DarkUI.Controls.DarkLabel();
            this.panel2 = new System.Windows.Forms.Panel();
            this.timerUpdateLedImage = new System.Windows.Forms.Timer(this.components);
            this.pictureBoxCm = new System.Windows.Forms.PictureBox();
            this.groupBox1.SuspendLayout();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ledBrightnessSlider)).BeginInit();
            this.panel3.SuspendLayout();
            this.panel4.SuspendLayout();
            this.panel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxCm)).BeginInit();
            this.SuspendLayout();
            // 
            // alwaysOnRadioBtn
            // 
            this.alwaysOnRadioBtn.AutoSize = true;
            this.alwaysOnRadioBtn.Location = new System.Drawing.Point(4, 9);
            this.alwaysOnRadioBtn.Name = "alwaysOnRadioBtn";
            this.alwaysOnRadioBtn.Size = new System.Drawing.Size(75, 17);
            this.alwaysOnRadioBtn.TabIndex = 1;
            this.alwaysOnRadioBtn.TabStop = true;
            this.alwaysOnRadioBtn.Text = "Always On";
            // 
            // alwaysOffRadioBtn
            // 
            this.alwaysOffRadioBtn.AutoSize = true;
            this.alwaysOffRadioBtn.Location = new System.Drawing.Point(113, 9);
            this.alwaysOffRadioBtn.Name = "alwaysOffRadioBtn";
            this.alwaysOffRadioBtn.Size = new System.Drawing.Size(75, 17);
            this.alwaysOffRadioBtn.TabIndex = 2;
            this.alwaysOffRadioBtn.TabStop = true;
            this.alwaysOffRadioBtn.Text = "Always Off";
            // 
            // breathingRadioBtn
            // 
            this.breathingRadioBtn.AutoSize = true;
            this.breathingRadioBtn.Location = new System.Drawing.Point(217, 9);
            this.breathingRadioBtn.Name = "breathingRadioBtn";
            this.breathingRadioBtn.Size = new System.Drawing.Size(70, 17);
            this.breathingRadioBtn.TabIndex = 3;
            this.breathingRadioBtn.TabStop = true;
            this.breathingRadioBtn.Text = "Breathing";
            // 
            // rapidFireRadioBtn
            // 
            this.rapidFireRadioBtn.AutoSize = true;
            this.rapidFireRadioBtn.Location = new System.Drawing.Point(312, 9);
            this.rapidFireRadioBtn.Name = "rapidFireRadioBtn";
            this.rapidFireRadioBtn.Size = new System.Drawing.Size(73, 17);
            this.rapidFireRadioBtn.TabIndex = 4;
            this.rapidFireRadioBtn.TabStop = true;
            this.rapidFireRadioBtn.Text = "Rapid Fire";
            // 
            // applyBtn
            // 
            this.applyBtn.Location = new System.Drawing.Point(576, 241);
            this.applyBtn.Name = "applyBtn";
            this.applyBtn.Padding = new System.Windows.Forms.Padding(5);
            this.applyBtn.Size = new System.Drawing.Size(75, 23);
            this.applyBtn.TabIndex = 5;
            this.applyBtn.Text = "Apply";
            this.applyBtn.Click += new System.EventHandler(this.applyBtn_Click);
            // 
            // defaultBtn
            // 
            this.defaultBtn.Location = new System.Drawing.Point(576, 212);
            this.defaultBtn.Name = "defaultBtn";
            this.defaultBtn.Padding = new System.Windows.Forms.Padding(5);
            this.defaultBtn.Size = new System.Drawing.Size(75, 23);
            this.defaultBtn.TabIndex = 6;
            this.defaultBtn.Text = "Default";
            this.defaultBtn.Click += new System.EventHandler(this.defaultBtn_Click);
            // 
            // startupHandlerBtn
            // 
            this.startupHandlerBtn.Location = new System.Drawing.Point(145, 212);
            this.startupHandlerBtn.Name = "startupHandlerBtn";
            this.startupHandlerBtn.Padding = new System.Windows.Forms.Padding(5);
            this.startupHandlerBtn.Size = new System.Drawing.Size(223, 31);
            this.startupHandlerBtn.TabIndex = 7;
            this.startupHandlerBtn.Text = "Restore LED Color On Windows Startup";
            this.startupHandlerBtn.Click += new System.EventHandler(this.startupHandlerBtn_Click);
            // 
            // lightnessColorSlider
            // 
            this.lightnessColorSlider.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.lightnessColorSlider.Location = new System.Drawing.Point(0, 3);
            this.lightnessColorSlider.Name = "lightnessColorSlider";
            this.lightnessColorSlider.NubStyle = Cyotek.Windows.Forms.ColorSliderNubStyle.TopLeft;
            this.lightnessColorSlider.Size = new System.Drawing.Size(385, 22);
            this.lightnessColorSlider.TabIndex = 10;
            this.lightnessColorSlider.TabStop = false;
            this.lightnessColorSlider.ValueChanged += new System.EventHandler(this.lightnessColorSlider_ValueChanged);
            // 
            // saturationColorSlider
            // 
            this.saturationColorSlider.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.saturationColorSlider.Location = new System.Drawing.Point(0, 3);
            this.saturationColorSlider.Name = "saturationColorSlider";
            this.saturationColorSlider.NubStyle = Cyotek.Windows.Forms.ColorSliderNubStyle.TopLeft;
            this.saturationColorSlider.Size = new System.Drawing.Size(385, 22);
            this.saturationColorSlider.TabIndex = 9;
            this.saturationColorSlider.TabStop = false;
            this.saturationColorSlider.ValueChanged += new System.EventHandler(this.saturationColorSlider_ValueChanged);
            // 
            // hueColorSlider
            // 
            this.hueColorSlider.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.hueColorSlider.Location = new System.Drawing.Point(0, 3);
            this.hueColorSlider.Name = "hueColorSlider";
            this.hueColorSlider.NubStyle = Cyotek.Windows.Forms.ColorSliderNubStyle.TopLeft;
            this.hueColorSlider.Size = new System.Drawing.Size(385, 22);
            this.hueColorSlider.TabIndex = 8;
            this.hueColorSlider.TabStop = false;
            this.hueColorSlider.ValueChanged += new System.EventHandler(this.hueColorSlider_ValueChanged);
            // 
            // colorGrid1
            // 
            this.colorGrid1.Columns = 8;
            this.colorGrid1.Location = new System.Drawing.Point(12, 203);
            this.colorGrid1.Name = "colorGrid1";
            this.colorGrid1.Palette = Cyotek.Windows.Forms.ColorPalette.None;
            this.colorGrid1.Size = new System.Drawing.Size(127, 75);
            this.colorGrid1.TabIndex = 11;
            this.colorGrid1.TabStop = false;
            this.colorGrid1.ColorChanged += new System.EventHandler(this.colorGrid1_ColorChanged);
            // 
            // darkSeparator1
            // 
            this.darkSeparator1.Dock = System.Windows.Forms.DockStyle.Top;
            this.darkSeparator1.Location = new System.Drawing.Point(0, 0);
            this.darkSeparator1.Name = "darkSeparator1";
            this.darkSeparator1.Size = new System.Drawing.Size(663, 2);
            this.darkSeparator1.TabIndex = 0;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.panel1);
            this.groupBox1.Controls.Add(this.darkLabel5);
            this.groupBox1.Controls.Add(this.darkLabel4);
            this.groupBox1.Controls.Add(this.ledBrightnessSlider);
            this.groupBox1.Controls.Add(this.darkLabel3);
            this.groupBox1.Controls.Add(this.panel3);
            this.groupBox1.Controls.Add(this.darkLabel2);
            this.groupBox1.Controls.Add(this.panel4);
            this.groupBox1.Controls.Add(this.darkLabel1);
            this.groupBox1.Controls.Add(this.panel2);
            this.groupBox1.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.groupBox1.Location = new System.Drawing.Point(145, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(506, 194);
            this.groupBox1.TabIndex = 12;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "LED Controller";
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.alwaysOnRadioBtn);
            this.panel1.Controls.Add(this.alwaysOffRadioBtn);
            this.panel1.Controls.Add(this.breathingRadioBtn);
            this.panel1.Controls.Add(this.rapidFireRadioBtn);
            this.panel1.Location = new System.Drawing.Point(111, 154);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(389, 34);
            this.panel1.TabIndex = 13;
            // 
            // darkLabel5
            // 
            this.darkLabel5.AutoSize = true;
            this.darkLabel5.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(220)))), ((int)(((byte)(220)))));
            this.darkLabel5.Location = new System.Drawing.Point(3, 165);
            this.darkLabel5.Name = "darkLabel5";
            this.darkLabel5.Size = new System.Drawing.Size(58, 13);
            this.darkLabel5.TabIndex = 17;
            this.darkLabel5.Text = "LED Mode";
            // 
            // darkLabel4
            // 
            this.darkLabel4.AutoSize = true;
            this.darkLabel4.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(220)))), ((int)(((byte)(220)))));
            this.darkLabel4.Location = new System.Drawing.Point(3, 92);
            this.darkLabel4.Name = "darkLabel4";
            this.darkLabel4.Size = new System.Drawing.Size(58, 13);
            this.darkLabel4.TabIndex = 16;
            this.darkLabel4.Text = "(L)ightness";
            // 
            // ledBrightnessSlider
            // 
            this.ledBrightnessSlider.Location = new System.Drawing.Point(111, 114);
            this.ledBrightnessSlider.Name = "ledBrightnessSlider";
            this.ledBrightnessSlider.Size = new System.Drawing.Size(385, 45);
            this.ledBrightnessSlider.TabIndex = 15;
            this.ledBrightnessSlider.TabStop = false;
            // 
            // darkLabel3
            // 
            this.darkLabel3.AutoSize = true;
            this.darkLabel3.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(220)))), ((int)(((byte)(220)))));
            this.darkLabel3.Location = new System.Drawing.Point(3, 61);
            this.darkLabel3.Name = "darkLabel3";
            this.darkLabel3.Size = new System.Drawing.Size(61, 13);
            this.darkLabel3.TabIndex = 15;
            this.darkLabel3.Text = "(S)aturation";
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.saturationColorSlider);
            this.panel3.Location = new System.Drawing.Point(111, 46);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(385, 28);
            this.panel3.TabIndex = 13;
            // 
            // darkLabel2
            // 
            this.darkLabel2.AutoSize = true;
            this.darkLabel2.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(220)))), ((int)(((byte)(220)))));
            this.darkLabel2.Location = new System.Drawing.Point(3, 27);
            this.darkLabel2.Name = "darkLabel2";
            this.darkLabel2.Size = new System.Drawing.Size(33, 13);
            this.darkLabel2.TabIndex = 14;
            this.darkLabel2.Text = "(H)ue";
            // 
            // panel4
            // 
            this.panel4.Controls.Add(this.hueColorSlider);
            this.panel4.Location = new System.Drawing.Point(111, 12);
            this.panel4.Name = "panel4";
            this.panel4.Size = new System.Drawing.Size(385, 28);
            this.panel4.TabIndex = 14;
            // 
            // darkLabel1
            // 
            this.darkLabel1.AutoSize = true;
            this.darkLabel1.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(220)))), ((int)(((byte)(220)))));
            this.darkLabel1.Location = new System.Drawing.Point(3, 123);
            this.darkLabel1.Name = "darkLabel1";
            this.darkLabel1.Size = new System.Drawing.Size(80, 13);
            this.darkLabel1.TabIndex = 13;
            this.darkLabel1.Text = "LED Brightness";
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.lightnessColorSlider);
            this.panel2.Location = new System.Drawing.Point(111, 80);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(385, 28);
            this.panel2.TabIndex = 13;
            // 
            // timerUpdateLedImage
            // 
            this.timerUpdateLedImage.Interval = 40;
            this.timerUpdateLedImage.Tick += new System.EventHandler(this.timerUpdateLedImage_Tick);
            // 
            // pictureBoxCm
            // 
            this.pictureBoxCm.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureBoxCm.Image = global::CmAlcorGUI.Properties.Resources.cmstorm_white_med_bd1;
            this.pictureBoxCm.Location = new System.Drawing.Point(12, 12);
            this.pictureBoxCm.Name = "pictureBoxCm";
            this.pictureBoxCm.Size = new System.Drawing.Size(128, 185);
            this.pictureBoxCm.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBoxCm.TabIndex = 0;
            this.pictureBoxCm.TabStop = false;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(663, 287);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.colorGrid1);
            this.Controls.Add(this.darkSeparator1);
            this.Controls.Add(this.startupHandlerBtn);
            this.Controls.Add(this.defaultBtn);
            this.Controls.Add(this.applyBtn);
            this.Controls.Add(this.pictureBoxCm);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "MainForm";
            this.Text = "CM Alcor LED Software";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ledBrightnessSlider)).EndInit();
            this.panel3.ResumeLayout(false);
            this.panel4.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxCm)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pictureBoxCm;
        private DarkUI.Controls.DarkRadioButton alwaysOnRadioBtn;
        private DarkUI.Controls.DarkRadioButton alwaysOffRadioBtn;
        private DarkUI.Controls.DarkRadioButton breathingRadioBtn;
        private DarkUI.Controls.DarkRadioButton rapidFireRadioBtn;
        private DarkUI.Controls.DarkButton applyBtn;
        private DarkUI.Controls.DarkButton defaultBtn;
        private DarkUI.Controls.DarkButton startupHandlerBtn;
        private Cyotek.Windows.Forms.HueColorSlider hueColorSlider;
        private Cyotek.Windows.Forms.SaturationColorSlider saturationColorSlider;
        private Cyotek.Windows.Forms.LightnessColorSlider lightnessColorSlider;
        private Cyotek.Windows.Forms.ColorGrid colorGrid1;
        private DarkUI.Controls.DarkSeparator darkSeparator1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Panel panel4;
        private System.Windows.Forms.TrackBar ledBrightnessSlider;
        private DarkUI.Controls.DarkLabel darkLabel1;
        private DarkUI.Controls.DarkLabel darkLabel2;
        private DarkUI.Controls.DarkLabel darkLabel3;
        private DarkUI.Controls.DarkLabel darkLabel4;
        private DarkUI.Controls.DarkLabel darkLabel5;
        private System.Windows.Forms.Timer timerUpdateLedImage;
        private System.Windows.Forms.Panel panel1;
    }
}