using System;
using System.Collections.Generic;
using System.Drawing;

namespace CmAlcorGUI
{
    /// <summary>
    /// Settings saved between app sessions.
    /// </summary>
    [Serializable]
    public class AppSettings
    {
        public List<Color> customColors;
    };
}
