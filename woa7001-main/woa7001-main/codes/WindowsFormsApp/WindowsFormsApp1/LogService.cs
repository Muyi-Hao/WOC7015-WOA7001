using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsFormsApp1
{
    public class LogService
    {
        FileStream LogFile;
        string logurl;
        public LogService() {
            logurl = "Log_" + DateTime.Now.Year+"_"+ DateTime.Now.Month + "_" + DateTime.Now.Day + "_" + DateTime.Now.Hour + "_" + DateTime.Now.Minute + "_" + DateTime.Now.Millisecond + ".txt";
            LogFile = new FileStream(logurl, FileMode.Append);
        }
        public void Log(string message)
        {
            byte[] bytes = Encoding.UTF8.GetBytes(message);
            foreach(byte b in bytes) { 
                LogFile.WriteByte(b);
            }
            LogFile.WriteByte(0x0A);
            ExitLog();
            LogFile = new FileStream(logurl, FileMode.Append);
        }
        public void ExitLog()
        {
            LogFile.Close();
        }
    }
}
