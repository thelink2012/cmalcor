using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CmAlcorGUI
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            if(true || Firmware.IsMousePresent())
            {
                Application.Run(new MainForm());
            }
            else
            {
                string message = String.Format("The CM Storm {0} mouse is not connected or couldn't be found.",
                                               Firmware.IsLibraryForMizar()? "Mizar" : "Alcor");
                MessageBox.Show(message, "CmAlcorGUI", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
    }
}
