
namespace WInFormApp
{
    partial class StartForm
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.btnGen = new System.Windows.Forms.Button();
            this.txtIdList = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btnGen
            // 
            this.btnGen.Location = new System.Drawing.Point(12, 385);
            this.btnGen.Name = "btnGen";
            this.btnGen.Size = new System.Drawing.Size(170, 64);
            this.btnGen.TabIndex = 0;
            this.btnGen.Text = "注册";
            this.btnGen.UseVisualStyleBackColor = true;
            this.btnGen.Click += new System.EventHandler(this.btnGen_Click);
            // 
            // txtIdList
            // 
            this.txtIdList.Location = new System.Drawing.Point(12, 12);
            this.txtIdList.Multiline = true;
            this.txtIdList.Name = "txtIdList";
            this.txtIdList.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtIdList.Size = new System.Drawing.Size(443, 346);
            this.txtIdList.TabIndex = 1;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(286, 385);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(170, 64);
            this.button1.TabIndex = 2;
            this.button1.Text = "注销";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // StartForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(11F, 24F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(468, 514);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.txtIdList);
            this.Controls.Add(this.btnGen);
            this.Name = "StartForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "TestForm";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnGen;
        private System.Windows.Forms.TextBox txtIdList;
        private System.Windows.Forms.Button button1;
    }
}

