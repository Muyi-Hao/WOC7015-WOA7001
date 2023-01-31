using System;
using System.Drawing;
using System.IO;
using System.IO.Ports;
using System.Text;
using System.Text.Json;
using System.Threading;
using System.Windows.Forms;
using AForge.Video.DirectShow;
using Newtonsoft.Json;

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        private bool CanAutoMode=false;
        private FilterInfoCollection _videoDevices;//摄像设备 
        private VideoCaptureDevice _videoCapture; //捕获设备源
        private Bitmap bitmap;//处理图片
        public SerialPinChangedEventHandler SerialPinChanged;
        public delegate void Displaydelegate(byte[] InputBuf);
        public Displaydelegate disp_delegate;
        public event SerialErrorReceivedEventHandler Error;
        public static SerialPort serialPort1 = new SerialPort();
        public string logstr;
        public int armstatus = 0;
        public LogService logService;
        public Form1()
        {
            InitializeComponent();
            disconnected();
            logService = new LogService();
            button2.Enabled = false;
            button4.Enabled = false;
            logService.Log("[System] Programe start");
            textBox4.Text = DateTime.Now + "     " + "[System] Programe start";
            disp_delegate = new Displaydelegate(DispUI);
            serialPort1.PinChanged += new SerialPinChangedEventHandler(serialChanged);
            //serialPort1.DataReceived += new SerialDataReceivedEventHandler(serialPort1_DataReceived);
        }
        private void serialChanged(object sender, SerialPinChangedEventArgs e)
        {
            MessageBox.Show("cahgned");
            disconnected();
        }
        private void connected()
        {
            button4.Enabled = true;
            button3.Enabled = true;
            button5.Enabled = true;
            button6.Enabled = true;
            button7.Enabled = true;
            button8.Enabled = true;
            button2.Enabled = false;
        }
        private void disconnected()
        {
            label8.Text = "NOT CONNECT";
            label8.ForeColor = System.Drawing.Color.Red;
            button4.Enabled = false;
            button2.Enabled = true;
            button3.Enabled = false;
            button5.Enabled = false;
            button6.Enabled = false;
            button7.Enabled = false;
            button8.Enabled = false;
            label6.Text = "UNKNOW";
            label6.BackColor = System.Drawing.Color.Yellow;
            textBox4.BackColor = System.Drawing.Color.Yellow;
        }
        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label3_Click(object sender, EventArgs e)
        {

        }

        private void button2_Click(object sender, EventArgs e)
        {
            string aaa = comboBox1.SelectedItem.ToString();
            serialPort1 = new SerialPort(aaa);
            serialPort1.BaudRate = 9600;
            serialPort1.Parity = Parity.None;
            serialPort1.StopBits = StopBits.One;
            serialPort1.DataBits = 8;
            serialPort1.Handshake = Handshake.None;
            serialPort1.RtsEnable = true;
            serialPort1.DataReceived += new SerialDataReceivedEventHandler(serialPort1_DataReceived);
            try
            {
                serialPort1.Open();
                MessageBox.Show("Connect Succeed");
                label8.Text = aaa + " IS OPEN";
                label8.ForeColor = System.Drawing.Color.Green;
                log("[System] Connect to " + aaa + " Succeed;");
                connected();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Connect FAILD");
                log(ex.Message);
            }
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            logService.Log("Exit");
            serialPort1.Close();
        }

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                Thread.Sleep(100);  //（毫秒）等待一定时间，确保数据的完整性 int len        
                int len = serialPort1.BytesToRead;
                if (len != 0)
                {
                    byte[] buff = new byte[len];
                    serialPort1.Read(buff, 0, len);
                    this.Invoke(disp_delegate, buff);
                }
            }
            catch (Exception ex)
            {
                log(ex.Message);
            }
        }
        public void log(string msg)
        {
            logService.Log(DateTime.Now + "    " + msg);
            this.textBox4.Text += Environment.NewLine + DateTime.Now + "     " + msg;
            this.textBox4.SelectionStart = this.textBox4.Text.Length;
            this.textBox4.ScrollToCaret();

        }
        public void DispUI(byte[] InputBuf)
        {
            //textBox1.Text = Convert.ToString(InputBuf);
            string aaa = Encoding.Default.GetString(InputBuf);
            //log("[Resive] " + aaa);
            anaylise(aaa);
        }
        private void anaylise(string msg)
        {
            MessageMember mm = System.Text.Json.JsonSerializer.Deserialize<MessageMember>(msg);
            if (mm.code == 0)  //code 0 : NOT RUNNING
            {
                label6.Text = "STOPED";
                log("[WARNNING] ARM STOPED");
                label6.BackColor = System.Drawing.Color.Red;
                textBox4.BackColor = System.Drawing.Color.Red;
            }
            else if (mm.code == 1) //code 1 : RUNNING
            {
                label6.Text = "RUNNING";
                log("[NOTICE] " + ": " + mm.msg);
                label6.BackColor = System.Drawing.Color.Green;
                textBox4.BackColor = System.Drawing.Color.Green;
            }
            else
            {

            }
        }
        private void button1_Click(object sender, EventArgs e)
        {
            comboBox1.Items.Clear();
            log("[System] Get COM Info");
            String[] portnames = SerialPort.GetPortNames();
            foreach (var item in portnames)
            {
                comboBox1.Items.Add(item);
            }
            if (comboBox1.Items.Count != 0)
            {
                button2.Enabled = true;
            }

        }

        public bool x_y_z_check()
        {
            try
            {
                int x = Convert.ToInt16(textBox1.Text);
                int y = Convert.ToInt16(textBox2.Text);
                int z = Convert.ToInt16(textBox3.Text);
                if (x >= 0 && x <= 180)
                {
                    if (y >= 0 && y <= 180)
                    {
                        if (z >= 0 && z <= 180)
                        {
                            return true;
                        }
                    }
                }
                return false;
            }
            catch
            {
                return false;
            }
        }
        private void button3_Click(object sender, EventArgs e)
        {
            if (x_y_z_check())
            {
                string snedtxt = Convert.ToInt16(textBox1.Text) + "," + Convert.ToInt16(textBox2.Text) + "," + Convert.ToInt16(textBox3.Text);
                try
                {
                    log("[Sent] x = " + Convert.ToInt16(textBox1.Text) + " , y = " + Convert.ToInt16(textBox2.Text) + " , z = " + Convert.ToInt16(textBox3.Text) + " succeed");
                    MessageMember message= new MessageMember();
                    message.code = 1;
                    message.msg= snedtxt;
                    serialPort1.Write(JsonConvert.SerializeObject(message));
                }
                catch (Exception ex)
                {
                    log(ex.Message);
                }
            }
            else
            {
                MessageBox.Show("Input X , Y, Z data error!");
                log("[System] Input X , Y, Z data error!");
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            log("[System] Disconnect");
            serialPort1.Close();
            disconnected();
        }

        /// <summary>
        /// 下面是USB相机类操作代码
        /// </summary>
        private void ini_viedo()
        {
            log("[System] Init Video Device");
            try
            {
                log("[System] Get Video Device Info");
                _videoDevices = new FilterInfoCollection(FilterCategory.VideoInputDevice);
                if(_videoDevices.Count == 0)
                {
                    log("[System] No Video Device Found");
                }
                else
                {
                    log("[System] Video Device Detected");
                    CameraConn();
                }
            }
            catch (Exception)
            {
                log("[System] No Video Device Found");
                _videoDevices = null;
            }
        }

        public void ShutCamera()
        {
            log("[System] Shutting down Device");
            if (videoSourcePlayer1.VideoSource != null)
            {
                videoSourcePlayer1.SignalToStop();
                videoSourcePlayer1.VideoSource.WaitForStop();
                videoSourcePlayer1.VideoSource = null;
            }
        }
        public void CameraConn()
        {
            log("[System] Connecting Device");
            _videoCapture = new VideoCaptureDevice(_videoDevices[0].MonikerString);
            videoSourcePlayer1.VideoSource = _videoCapture;
            videoSourcePlayer1.Start();
            log("[System] Connect Device Succeed");
            CanAutoMode = true;
        }



    }
}
