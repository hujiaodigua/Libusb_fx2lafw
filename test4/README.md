采集数据得到data.txt指令 
-- ./main data.txt
处理data.txt得到mailbox_samples.txt指令
-- python file_bin2hex.py
处理mailbox_samples.txt得到mailbox_samples.vcd指令
-- python pyusb_sample_fx2_vcd.py mailbox_samples.txt >> mailbox_samples.vcd