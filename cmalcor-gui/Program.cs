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

            if(Firmware.IsAlcorPresent())
            {
                Application.Run(new MainForm());
            }
            else
            {
                MessageBox.Show("The CM Storm Alcor mouse is not connected or couldn't be found.",
                                "CmAlcorGUI", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
    }
}
