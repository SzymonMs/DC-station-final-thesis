using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;
using System.Globalization;
using System.IO;
using System.Text.RegularExpressions;
using System.Windows.Forms.DataVisualization.Charting;

namespace TestUART
{
    public partial class Form1 : Form
    {
        string dataTransmit;
        string dataRecived;

        double time = 0.0;
        double timeMax = 8.0;
        double dataI;
        double dataW;
        int signOfDataMax = 8;

        string fullPathI = @"C:\Temp\pomiarI.csv";
        string fullPathW = @"C:\Temp\pomiarW.csv";
        string fullPathU = @"C:\Temp\pomiarU.csv";
        string fullPathDuty = @"C:\Temp\pomiarDuty.csv";

        public Form1()
        {
            InitializeComponent();
        }
        private void Form1_Load(object sender, EventArgs e)
        {
            string[] ports = SerialPort.GetPortNames();
            cBoxPort.Items.AddRange(ports);
            statusPort.Enabled = false;
            tbSlownik.Text = "INSTRUKCJA NASTAWIANIA PARAMETRÓW\r\n\r\n\r\n" +
                "Zadawanie wypełnienia dla pętli otwartej:\r\n" +
                "np. PWM00100-100%, PWM00055-55%.\r\n\r\n" +
                "Zadawanie wartości prądu dla regulacji prądu:\r\n" +
                "np. IZZ00700-700mA, IZZ01500-1.5A.\r\n\r\n" +
                "Zadawanie prędkości dla kaskadowej regulacji prędkości:\r\n" +
                "np. WZZ00700-700rpm, WZZ01500-1500rpm.\r\n\r\n" +
                "Zmianianie nastaw regulatora:\r\n" +
                "np. KPW12345 ustawia wartość KPW na 123.45, KII00989 ustawia wartość KII na 9.89.\r\n" +
                "Analogicznie ustawiać pozostałe nastawy.\r\n\r\n" +
                "Aby obserwować przebiegi prądu i prędkości, w zakładce pomiary, należy włączyć rejestrację danych!";
            System.IO.File.WriteAllText(fullPathU, string.Empty);
            System.IO.File.WriteAllText(fullPathI, string.Empty);
            System.IO.File.WriteAllText(fullPathW, string.Empty);
            System.IO.File.WriteAllText(fullPathDuty, string.Empty);

            chart1.ChartAreas[0].AxisX.LabelStyle.Format = "0.0";
            chart2.ChartAreas[0].AxisX.LabelStyle.Format = "0.0";

            chart1.ChartAreas[0].AxisY.LabelStyle.Format = "0.0";
            chart2.ChartAreas[0].AxisY.LabelStyle.Format = "0.0";

            chart1.ChartAreas[0].AxisX.Title = "t [s]";
            chart1.ChartAreas[0].AxisY.TitleAlignment = System.Drawing.StringAlignment.Far; 
            chart1.ChartAreas[0].AxisY.TextOrientation = TextOrientation.Horizontal;
            chart2.ChartAreas[0].AxisY.TitleAlignment = System.Drawing.StringAlignment.Far; 
            chart2.ChartAreas[0].AxisY.TextOrientation = TextOrientation.Horizontal;
            chart2.ChartAreas[0].AxisX.Title = "t [s]";

            chart1.ChartAreas[0].AxisY.Title = "I [A]";
            chart2.ChartAreas[0].AxisY.Title = "n [rpm]";

            chart1.ChartAreas[0].AxisX.Maximum = 8.0;
            chart2.ChartAreas[0].AxisX.Maximum = 8.0;

            chart1.ChartAreas[0].AxisY.Maximum = 3.5;
            chart2.ChartAreas[0].AxisY.Maximum = 2500.0;

            chart1.ChartAreas[0].AxisY.MajorGrid.Interval = 0.5;
            chart2.ChartAreas[0].AxisY.MajorGrid.Interval = 500;
        }

        private void btnOpen_Click(object sender, EventArgs e)
        {
            try
            {
                serialPort1.PortName = cBoxPort.Text;
                serialPort1.BaudRate = Convert.ToInt32(cBoxBaudeRate.Text);
                serialPort1.DataBits = Convert.ToInt32(cBoxData.Text);
                serialPort1.StopBits = (StopBits)Enum.Parse(typeof(StopBits), cBoxStop.Text);
                serialPort1.Parity = (Parity)Enum.Parse(typeof(Parity), cBoxParity.Text);

                serialPort1.Open();

                statusPort.Text = "ON";
                statusPort.ForeColor = Color.Green;
                statusPort.Enabled = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Błąd!", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        private void btnClose_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                serialPort1.Close();
                statusPort.Text = "OFF";
                statusPort.ForeColor = Color.Red;
            }
        }
        private void btnSend_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                dataTransmit = tBoxTransmit.Text;
                if (dataTransmit.Length == signOfDataMax)
                {
                    serialPort1.Write(dataTransmit);
                }
            }
        }
        private void button1_Click(object sender, EventArgs e)
        {
            tBoxTransmit.Text = "";
        }
        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            dataRecived = serialPort1.ReadLine();
            serialPort1.DiscardInBuffer();
            serialPort1.DiscardOutBuffer();
            this.Invoke(new EventHandler(ShowData));
        }
        private void ShowData(object sender, EventArgs e)
        {
            string[] tab;
            tab = dataRecived.Split(' ');
            tBoxRecive.Text = tab[0];
            if (tab[0] == "parm")
            {
                textBox1.Text = tab[1];
                textBox2.Text = tab[2];
                textBox3.Text = tab[3];
                textBox4.Text = tab[4];
                textBox5.Text = tab[5];
                textBox6.Text = tab[6];
                textBox7.Text = tab[7];
                if (tab[8] == "0")
                {
                    textBox8.Text = "OTWARTY";
                }
                else if (tab[8] == "1")
                {
                    textBox8.Text = "PRĄDU";
                }
                else if (tab[8] == "2")
                {
                    textBox8.Text = "KASKADOWY";
                }

            }
            else if (tab[0] == "data")
            {
                textBox9.Text = tab[1]; //i(t)
                textBox10.Text = tab[2]; //w(t)
                textBox11.Text = tab[3]; //u(t)
                textBox12.Text = tab[4]; //duty(t)

                using (StreamWriter sw = File.AppendText(fullPathI))
                {
                    sw.WriteLine(time.ToString("0.00") + " " + textBox9.Text);
                }
                using (StreamWriter sw = File.AppendText(fullPathW))
                {
                    sw.WriteLine(time.ToString("0.00") + " " + textBox10.Text);
                }
                using (StreamWriter sw = File.AppendText(fullPathU))
                {
                    sw.WriteLine(time.ToString("0.00") + " " + textBox11.Text);
                }
                using (StreamWriter sw = File.AppendText(fullPathDuty))
                {
                    sw.WriteLine(time.ToString("0.00") + " " + textBox12.Text);
                }

                dataI = double.Parse(tab[1]) / 1000.0;
                dataW = double.Parse(tab[2]);

                chart1.Series[0].Points.AddXY(time, dataI);
                chart2.Series[0].Points.AddXY(time, dataW);

                if (time > timeMax)
                {
                    chart1.Series[0].Points.RemoveAt(0);
                    chart1.ChartAreas[0].AxisX.Minimum = time - timeMax;
                    chart1.ChartAreas[0].AxisX.Maximum = time;
                    chart2.Series[0].Points.RemoveAt(0);
                    chart2.ChartAreas[0].AxisX.Minimum = time - timeMax;
                    chart2.ChartAreas[0].AxisX.Maximum = time;
                }
                if (dataW < -50.0)
                {
                    chart2.ChartAreas[0].AxisY.Minimum = -2500.0;
                }

                time = time + 0.01;
            }
        }

        private void btnClearRecive_Click(object sender, EventArgs e)
        {

            tBoxRecive.Text = "";
        }

        private void btnKaskada_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                dataTransmit = "REG00002";
                serialPort1.Write(dataTransmit);
            }
        }

        private void btnPrad_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                dataTransmit = "REG00001";
                serialPort1.Write(dataTransmit);
            }
        }

        private void btnOtwarty_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                dataTransmit = "REG00000";
                serialPort1.Write(dataTransmit);
            }
        }

        private void textBox7_TextChanged(object sender, EventArgs e)
        {

        }

        private void btnResetDanych_Click(object sender, EventArgs e)
        {
            System.IO.File.WriteAllText(fullPathU, string.Empty);
            System.IO.File.WriteAllText(fullPathI, string.Empty);
            System.IO.File.WriteAllText(fullPathW, string.Empty);
            System.IO.File.WriteAllText(fullPathDuty, string.Empty);
            time = 0.0;
        }

        private void btnRescan_Click(object sender, EventArgs e)
        {
            cBoxPort.Items.Clear();
            string[] ports = SerialPort.GetPortNames();
            cBoxPort.Items.AddRange(ports);
            statusPort.Enabled = false;
        }

        private void btnON_Click_2(object sender, EventArgs e)
        {
            //chart1.Series[0].Points.Clear();
            //chart2.Series[0].Points.Clear();
            //time = 0.0;
            if (serialPort1.IsOpen)
            {
                dataTransmit = "DON00000";
                serialPort1.Write(dataTransmit);
            }
        }

        private void btnOFF_Click_2(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                dataTransmit = "DOF00000";
                serialPort1.Write(dataTransmit);
            }
        }

        private void btnResetDanych_Click_1(object sender, EventArgs e)
        {
            System.IO.File.WriteAllText(fullPathU, string.Empty);
            System.IO.File.WriteAllText(fullPathI, string.Empty);
            System.IO.File.WriteAllText(fullPathW, string.Empty);
            System.IO.File.WriteAllText(fullPathDuty, string.Empty);
            chart1.Series[0].Points.Clear();
            chart2.Series[0].Points.Clear();
            chart1.ChartAreas[0].AxisX.Minimum = 0.0;
            chart1.ChartAreas[0].AxisX.Maximum = 8.0;
            chart2.ChartAreas[0].AxisX.Minimum = 0.0;
            chart2.ChartAreas[0].AxisX.Maximum = 8.0;
            chart2.ChartAreas[0].AxisY.Minimum = 0.0;
            chart2.ChartAreas[0].AxisY.Maximum = 2500.0;
            time = 0.0;
        }
    }
}
