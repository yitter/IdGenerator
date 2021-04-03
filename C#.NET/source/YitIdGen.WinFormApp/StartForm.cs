using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WInFormApp
{
    public partial class StartForm : Form
    {
        public StartForm()
        {
            InitializeComponent();
        }

        [DllImport("yitidgengo.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern long NextId();

        [DllImport("yitidgengo.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern long NextId2();

        [DllImport("yitidgengo.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern IntPtr RegisterMany(string ip, int port, string password, int maxWorkerIdNumber, int idCount);
        //public static extern ulong RegisterWorkerId2();

        [DllImport("yitidgengo.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern void UnRegister();

        [DllImport("yitidgen.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern void SetWorkerId(uint workerId);

        [DllImport("yitidgen.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern int Test();

        [DllImport("yitidgen.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern int GetWorkerId(string ip, int port);


        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void btnGen_Click(object sender, EventArgs e)
        {
            try
            {
                var ip = "localhost";

                //txtIdList.Text += RegisterWorkerId(Encoding.UTF8.GetBytes(ip), 6379) + "\r\n";
                var ids = RegisterMany(ip, 6379, "", 4, 3);
                //foreach (var id in ids)
                //{
                //    txtIdList.Text += id;
                //}

                //txtIdList.Text += RegisterWorkerId() + "\r\n";

                //txtIdList.Text += Test() + "\r\n";
            }
            catch (Exception ex)
            {
                txtIdList.Text = ex.Message;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                UnRegister();
                txtIdList.Text += "LogOff";
            }
            catch (Exception ex)
            {
                txtIdList.Text = ex.Message;
            }
            // GetWorkerId("localhost", 6379);
        }
    }
}
