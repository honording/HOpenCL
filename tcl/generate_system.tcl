set typeACC		convolution
set numGroup	4
set numGPP		1
set numACC		3
#====================================
#Create Block Design
#====================================
create_bd_design "system"
#Add HLS IP Respo
#====================================
#Create Host
#====================================
create_bd_cell -type ip -vlnv xilinx.com:ip:microblaze:9.3 Host
#====================================
#Add Global Peripheral
#====================================
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_timer:2.0 axi_timer_0
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_uartlite:2.0 axi_uartlite_0
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_hwicap:3.0 axi_hwicap_0
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.0 GSMem
set_property -dict [list CONFIG.SINGLE_PORT_BRAM {1}] [get_bd_cells GSMem]
create_bd_cell -type ip -vlnv xilinx.com:ip:mig_7series:2.1 GlobalMem
create_bd_cell -type ip -vlnv xilinx.com:hls:groupIDGenerator:1.0 groupIDGenerator_0
create_bd_cell -type ip -vlnv xilinx.com:hls:groupNumberAllocator:1.0 groupNumberAllocator_0
create_bd_cell -type ip -vlnv xilinx.com:ip:axis_interconnect:2.1 axis_interconnect_0
set_property -dict [list CONFIG.NUM_SI "$numGroup" CONFIG.NUM_MI {1}] [get_bd_cells axis_interconnect_0]
create_bd_cell -type ip -vlnv xilinx.com:ip:fifo_generator:12.0 FIFO_0
set_property -dict [list CONFIG.INTERFACE_TYPE {AXI_STREAM} CONFIG.TDATA_NUM_BYTES {4} CONFIG.TUSER_WIDTH {0} CONFIG.Input_Depth_axis {16}] [get_bd_cells FIFO_0]

#Add LocalCoreIDAllocator
for {set i 0} {$i < $numGroup} {incr i} \
{
	create_bd_cell -type ip -vlnv xilinx.com:hls:localCoreIDAllocator:1.0 localCoreIDAllocator_$i
}
apply_bd_automation -rule xilinx.com:bd_rule:mig_7series -config {Board_Interface "ddr3_sdram" }  [get_bd_cells GlobalMem]
apply_bd_automation -rule xilinx.com:bd_rule:microblaze -config {local_mem "32KB" ecc "None" cache "None" debug_module "Debug Only" axi_periph "Enabled" axi_intc "0" clk "/GlobalMem/ui_clk (100 MHz)" }  [get_bd_cells Host]
apply_bd_automation -rule xilinx.com:bd_rule:board -config {Board_Interface "reset" }  [get_bd_pins GlobalMem/sys_rst]

apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config {Master "/Host (Periph)" Clk "Auto" }  [get_bd_intf_pins axi_timer_0/S_AXI]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config {Master "/Host (Periph)" Clk "Auto" }  [get_bd_intf_pins axi_uartlite_0/S_AXI]
apply_bd_automation -rule xilinx.com:bd_rule:board -config {Board_Interface "rs232_uart" }  [get_bd_intf_pins axi_uartlite_0/UART]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config {Master "/Host (Periph)" Clk "Auto" }  [get_bd_intf_pins axi_hwicap_0/S_AXI_LITE]
connect_bd_net -net [get_bd_nets Host_Clk] [get_bd_pins axi_hwicap_0/icap_clk] [get_bd_pins GlobalMem/ui_clk]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config {Master "/Host (Periph)" Clk "Auto" }  [get_bd_intf_pins GSMem/S_AXI]
apply_bd_automation -rule xilinx.com:bd_rule:bram_cntlr -config {BRAM "New Blk_Mem_Gen" }  [get_bd_intf_pins GSMem/BRAM_PORTA]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config {Master "/Host (Periph)" Clk "Auto" }  [get_bd_intf_pins GlobalMem/S_AXI]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config {Master "/Host (Periph)" Clk "Auto" }  [get_bd_intf_pins groupIDGenerator_0/s_axi_BUS_CMD]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config {Master "/Host (Periph)" Clk "Auto" }  [get_bd_intf_pins groupNumberAllocator_0/s_axi_BUS_CMD]
for {set i 0} {$i < $numGroup} {incr i} \
{
	apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config {Master "/Host (Periph)" Clk "Auto" }  [get_bd_intf_pins localCoreIDAllocator_$i/s_axi_BUS_CMD]
}
#====================================
#Add Groups
#====================================
for {set i 0} {$i < $numGroup} {incr i} \
{
	#Add DMA
	create_bd_cell -type ip -vlnv xilinx.com:ip:axi_cdma:4.1 DMA_Group$i
	set_property -dict [list CONFIG.C_INCLUDE_SG {0}] [get_bd_cells DMA_Group$i]
	#Add LocalIDGenerator
	create_bd_cell -type ip -vlnv xilinx.com:hls:localIDGenerator:1.0 localIDGenerator_$i
	#Add Barrier
	create_bd_cell -type ip -vlnv xilinx.com:hls:barrier:1.0 barrier_$i
	#Add LocalMem
	create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.0 LocalMem_$i
	set_property -dict [list CONFIG.SINGLE_PORT_BRAM {1}] [get_bd_cells LocalMem_$i]
	#Add LSMem
	create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.0 LSMem_$i
	set_property -dict [list CONFIG.SINGLE_PORT_BRAM {1}] [get_bd_cells LSMem_$i]
	#Add General-purpose Processor/MicroBlaze
	for {set j 0} {$j < $numGPP} {incr j} \
	{
		create_bd_cell -type ip -vlnv xilinx.com:ip:microblaze:9.3 Slave$j\_Group$i
		apply_bd_automation -rule xilinx.com:bd_rule:microblaze -config {local_mem "32KB" ecc "None" cache "None" debug_module "Debug Only" axi_periph "Enabled" axi_intc "0" clk "/GlobalMem/ui_clk (100 MHz)" }  [get_bd_cells Slave$j\_Group$i]
		if  { $j==0 } \
		{
			set_property -dict [list CONFIG.G_TEMPLATE_LIST {2} CONFIG.C_FSL_LINKS {7} CONFIG.C_USE_ICACHE {0} CONFIG.C_USE_DCACHE {0}] [get_bd_cells Slave$j\_Group$i]
		} \
		else \
		{
			set_property -dict [list CONFIG.G_TEMPLATE_LIST {2} CONFIG.C_FSL_LINKS {4} CONFIG.C_USE_ICACHE {0} CONFIG.C_USE_DCACHE {0}] [get_bd_cells Slave$j\_Group$i]
		}
	}
	for {set k 0} {$k < $numACC} {incr k} \
	{
		create_bd_cell -type ip -vlnv xilinx.com:hls:$typeACC\:1.0 $typeACC$k\_Group$i
	}
	#Connect Peripheral inside Group
	apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config "Master \"/Slave0_Group$i (Periph)\" Clk \"Auto\" "  [get_bd_intf_pins DMA_Group$i/S_AXI_LITE]
	apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config "Master \"/Slave0_Group$i (Periph)\" Clk \"Auto\" "  [get_bd_intf_pins localIDGenerator_$i/s_axi_BUS_CMD]
	apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config "Master \"/Slave0_Group$i (Periph)\" Clk \"Auto\" "  [get_bd_intf_pins barrier_$i/s_axi_BUS_CMD]
	apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config "Master \"/Slave0_Group$i (Periph)\" Clk \"Auto\" "  [get_bd_intf_pins LocalMem_$i/S_AXI]
	apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config "Master \"/Slave0_Group$i (Periph)\" Clk \"Auto\" "  [get_bd_intf_pins LSMem_$i/S_AXI]
	apply_bd_automation -rule xilinx.com:bd_rule:bram_cntlr -config {BRAM "New Blk_Mem_Gen" }  [get_bd_intf_pins LocalMem_$i/BRAM_PORTA]
	apply_bd_automation -rule xilinx.com:bd_rule:bram_cntlr -config {BRAM "New Blk_Mem_Gen" }  [get_bd_intf_pins LSMem_$i/BRAM_PORTA]
	apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config "Slave \"/LocalMem_$i/S_AXI\" Clk \"Auto\" "  [get_bd_intf_pins DMA_Group$i/M_AXI]
	create_bd_cell -type ip -vlnv xilinx.com:ip:axis_interconnect:2.1 axis_interconnect_G$i
	set_property -dict [list CONFIG.NUM_SI "[expr $numGPP+$numACC]" CONFIG.NUM_MI {1}] [get_bd_cells axis_interconnect_G$i]
	create_bd_cell -type ip -vlnv xilinx.com:ip:fifo_generator:12.0 FIFO_G$i
	set_property -dict [list CONFIG.INTERFACE_TYPE {AXI_STREAM} CONFIG.TDATA_NUM_BYTES {4} CONFIG.TUSER_WIDTH {0} CONFIG.Input_Depth_axis {16}] [get_bd_cells FIFO_G$i]

	#Connect Peripheral with GPP
	for {set k 1} {$k < $numGPP} {incr k} \
	{
		apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config "Slave \"/LocalMem_$i/S_AXI\" Clk \"Auto\" "  [get_bd_intf_pins Slave$k\_Group$i/M_AXI_DP]		
	}	
	#Connect Peripheral with ACC
	for {set k 0} {$k < $numACC} {incr k} \
	{
		apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config "Slave \"/LocalMem_$i/S_AXI\" Clk \"Auto\" "  [get_bd_intf_pins $typeACC$k\_Group$i/m_axi_data]
	}
	#Add FIFOs
	for {set k 0} {$k < [expr ($numGPP+$numACC)*5+3]} {incr k} \
	{
		create_bd_cell -type ip -vlnv xilinx.com:ip:fifo_generator:12.0 FIFO$k\_Group$i
		set_property -dict [list CONFIG.INTERFACE_TYPE {AXI_STREAM} CONFIG.TDATA_NUM_BYTES {4} CONFIG.TUSER_WIDTH {0} CONFIG.Input_Depth_axis {16}] [get_bd_cells FIFO$k\_Group$i]
	}	
	#Connect AXI_STREAM
	set f		0
	set core	0
	#Connect GPP
	for {set k 0} {$k < $numGPP} {incr k} \
	{
		#Connect localCoreIDAllocator
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/S_AXIS] [get_bd_intf_pins localCoreIDAllocator_$i/core$core] 
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/M_AXIS] [get_bd_intf_pins Slave$k\_Group$i/S0_AXIS]		
		connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins FIFO$f\_Group$i/s_aclk] [get_bd_pins GlobalMem/ui_clk]
		connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins FIFO$f\_Group$i/s_aresetn] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]
		incr f
		#Connect Barrier Out
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/S_AXIS] [get_bd_intf_pins barrier_$i/ocore$core]
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/M_AXIS] [get_bd_intf_pins Slave$k\_Group$i/S1_AXIS]
		connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins FIFO$f\_Group$i/s_aclk] [get_bd_pins GlobalMem/ui_clk]
		connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins FIFO$f\_Group$i/s_aresetn] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]	
		incr f
		#Connect Barrier In
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/S_AXIS] [get_bd_intf_pins Slave$k\_Group$i/M1_AXIS] 
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/M_AXIS] [get_bd_intf_pins barrier_$i/icore$core]
		connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins FIFO$f\_Group$i/s_aclk] [get_bd_pins GlobalMem/ui_clk]
		connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins FIFO$f\_Group$i/s_aresetn] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]			
		incr f
		#Connect localIDGenerator X
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/S_AXIS] [get_bd_intf_pins localIDGenerator_$i/id$core\x]
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/M_AXIS] [get_bd_intf_pins Slave$k\_Group$i/S2_AXIS]
		connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins FIFO$f\_Group$i/s_aclk] [get_bd_pins GlobalMem/ui_clk]
		connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins FIFO$f\_Group$i/s_aresetn] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]
		incr f
		#Connect localIDGenerator Y
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/S_AXIS] [get_bd_intf_pins localIDGenerator_$i/id$core\y]
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/M_AXIS] [get_bd_intf_pins Slave$k\_Group$i/S3_AXIS]
		connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins FIFO$f\_Group$i/s_aclk] [get_bd_pins GlobalMem/ui_clk]
		connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins FIFO$f\_Group$i/s_aresetn] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]
		incr f
		#Connect localIDGenerator CoreID
		connect_bd_intf_net [get_bd_intf_pins Slave$k\_Group$i/M2_AXIS] -boundary_type upper [get_bd_intf_pins axis_interconnect_G$i/S[format "%02d" $core]_AXIS]
		connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins axis_interconnect_G$i/S[format "%02d" $core]_AXIS_ACLK] [get_bd_pins GlobalMem/ui_clk]
		connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins axis_interconnect_G$i/S[format "%02d" $core]_AXIS_ARESETN] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]	
		
		incr core
	}
	#Handle Extra LocalHost AXI_STREAM
	#Connect groupIDGenerator X
	connect_bd_intf_net [get_bd_intf_pins groupIDGenerator_0/id$i\x] [get_bd_intf_pins FIFO$f\_Group$i/S_AXIS]
	connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/M_AXIS] [get_bd_intf_pins Slave0_Group$i/S4_AXIS]
	connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins FIFO$f\_Group$i/s_aclk] [get_bd_pins GlobalMem/ui_clk]
	connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins FIFO$f\_Group$i/s_aresetn] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]
	incr f
	#Connect groupIDGenerator Y
	connect_bd_intf_net [get_bd_intf_pins groupIDGenerator_0/id$i\y] [get_bd_intf_pins FIFO$f\_Group$i/S_AXIS]
	connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/M_AXIS] [get_bd_intf_pins Slave0_Group$i/S5_AXIS]
	connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins FIFO$f\_Group$i/s_aclk] [get_bd_pins GlobalMem/ui_clk]
	connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins FIFO$f\_Group$i/s_aresetn] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]
	incr f
	#Connect groupIDGenerator CoreID
	connect_bd_intf_net [get_bd_intf_pins Slave0_Group$i/M3_AXIS] -boundary_type upper [get_bd_intf_pins axis_interconnect_0/S[format "%02d" $i]_AXIS]
	connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins axis_interconnect_0/S[format "%02d" $i]_AXIS_ACLK] [get_bd_pins GlobalMem/ui_clk]
	connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins axis_interconnect_0/S[format "%02d" $i]_AXIS_ARESETN] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]	
	#Connect groupNumberAllocator
	connect_bd_intf_net [get_bd_intf_pins groupNumberAllocator_0/group$i] [get_bd_intf_pins FIFO$f\_Group$i/S_AXIS]
	connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/M_AXIS] [get_bd_intf_pins Slave0_Group$i/S6_AXIS]
	connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins FIFO$f\_Group$i/s_aclk] [get_bd_pins GlobalMem/ui_clk]
	connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins FIFO$f\_Group$i/s_aresetn] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]
	incr f
	#Connect ACC
	for {set k 0} {$k < $numACC} {incr k} \
	{
		#Connect localCoreIDAllocator
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/S_AXIS] [get_bd_intf_pins localCoreIDAllocator_$i/core$core] 
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/M_AXIS] [get_bd_intf_pins $typeACC$k\_Group$i/coreNum]		
		connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins FIFO$f\_Group$i/s_aclk] [get_bd_pins GlobalMem/ui_clk]
		connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins FIFO$f\_Group$i/s_aresetn] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]
		incr f
		#Connect Barrier Out
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/S_AXIS] [get_bd_intf_pins barrier_$i/ocore$core]
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/M_AXIS] [get_bd_intf_pins $typeACC$k\_Group$i/barrierOut]
		connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins FIFO$f\_Group$i/s_aclk] [get_bd_pins GlobalMem/ui_clk]
		connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins FIFO$f\_Group$i/s_aresetn] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]	
		incr f
		#Connect Barrier In
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/S_AXIS] [get_bd_intf_pins $typeACC$k\_Group$i/barrierIn] 
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/M_AXIS] [get_bd_intf_pins barrier_$i/icore$core]
		connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins FIFO$f\_Group$i/s_aclk] [get_bd_pins GlobalMem/ui_clk]
		connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins FIFO$f\_Group$i/s_aresetn] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]			
		incr f
		#Connect localIDGenerator X
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/S_AXIS] [get_bd_intf_pins localIDGenerator_$i/id$core\x]
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/M_AXIS] [get_bd_intf_pins $typeACC$k\_Group$i/localID0]
		connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins FIFO$f\_Group$i/s_aclk] [get_bd_pins GlobalMem/ui_clk]
		connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins FIFO$f\_Group$i/s_aresetn] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]
		incr f
		#Connect localIDGenerator Y
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/S_AXIS] [get_bd_intf_pins localIDGenerator_$i/id$core\y]
		connect_bd_intf_net [get_bd_intf_pins FIFO$f\_Group$i/M_AXIS] [get_bd_intf_pins $typeACC$k\_Group$i/localID1]
		connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins FIFO$f\_Group$i/s_aclk] [get_bd_pins GlobalMem/ui_clk]
		connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins FIFO$f\_Group$i/s_aresetn] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]
		incr f
		#Connect localIDGenerator CoreID
		connect_bd_intf_net [get_bd_intf_pins $typeACC$k\_Group$i/localIDout] -boundary_type upper [get_bd_intf_pins axis_interconnect_G$i/S[format "%02d" $core]_AXIS]
		connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins axis_interconnect_G$i/S[format "%02d" $core]_AXIS_ACLK] [get_bd_pins GlobalMem/ui_clk]
		connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins axis_interconnect_G$i/S[format "%02d" $core]_AXIS_ARESETN] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]	
		
		incr core
	}	
	#Handle localIDGenerator in coreID
	connect_bd_intf_net -boundary_type upper [get_bd_intf_pins axis_interconnect_G$i/M00_AXIS] [get_bd_intf_pins FIFO_G$i/S_AXIS]
	connect_bd_intf_net [get_bd_intf_pins FIFO_G$i/M_AXIS] [get_bd_intf_pins localIDGenerator_$i/coreID]
	connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins FIFO_G$i/s_aclk] [get_bd_pins GlobalMem/ui_clk]
	connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins FIFO_G$i/s_aresetn] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]
	connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins axis_interconnect_G$i/ACLK] [get_bd_pins GlobalMem/ui_clk]
	connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_interconnect_aresetn] [get_bd_pins axis_interconnect_G$i/ARESETN] [get_bd_pins rst_GlobalMem_100M/interconnect_aresetn]	
	connect_bd_net -net [get_bd_nets Slave0_Group$i\_Clk] [get_bd_pins axis_interconnect_G$i/M00_AXIS_ACLK] [get_bd_pins GlobalMem/ui_clk]
	connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins axis_interconnect_G$i/M00_AXIS_ARESETN] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]
}
#Handle groupIDGenerator in coreID
connect_bd_intf_net -boundary_type upper [get_bd_intf_pins axis_interconnect_0/M00_AXIS] [get_bd_intf_pins FIFO_0/S_AXIS]
connect_bd_intf_net [get_bd_intf_pins FIFO_0/M_AXIS] [get_bd_intf_pins groupIDGenerator_0/groupID]
connect_bd_net -net [get_bd_nets Host_Clk] [get_bd_pins axis_interconnect_0/ACLK] [get_bd_pins GlobalMem/ui_clk]
connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_interconnect_aresetn] [get_bd_pins axis_interconnect_0/ARESETN] [get_bd_pins rst_GlobalMem_100M/interconnect_aresetn]
connect_bd_net -net [get_bd_nets Host_Clk] [get_bd_pins FIFO_0/s_aclk] [get_bd_pins GlobalMem/ui_clk]
connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins FIFO_0/s_aresetn] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]
connect_bd_net -net [get_bd_nets Host_Clk] [get_bd_pins axis_interconnect_0/M00_AXIS_ACLK] [get_bd_pins GlobalMem/ui_clk]
connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins axis_interconnect_0/M00_AXIS_ARESETN] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]
#Connect all groups to Global Mem
set_property -dict [list CONFIG.NUM_SI "[expr $numGroup+1]"] [get_bd_cells Host_axi_periph]
for {set k 0} {$k < $numGroup} {incr k} \
{
	set_property -dict [list CONFIG.NUM_MI {6}] [get_bd_cells Slave0_Group$k\_axi_periph]
	connect_bd_net -net [get_bd_nets Host_Clk] [get_bd_pins Slave0_Group$k\_axi_periph/M05_ACLK] [get_bd_pins GlobalMem/ui_clk]
	connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins Slave0_Group$k\_axi_periph/M05_ARESETN] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]
	connect_bd_net -net [get_bd_nets Host_Clk] [get_bd_pins Host_axi_periph/S[format "%02d" [expr $k+1]]_ACLK] [get_bd_pins GlobalMem/ui_clk]
	connect_bd_net -net [get_bd_nets rst_GlobalMem_100M_peripheral_aresetn] [get_bd_pins Host_axi_periph/S[format "%02d" [expr $k+1]]_ARESETN] [get_bd_pins rst_GlobalMem_100M/peripheral_aresetn]
	connect_bd_intf_net -boundary_type upper [get_bd_intf_pins Slave0_Group$k\_axi_periph/M05_AXI] [get_bd_intf_pins Host_axi_periph/S[format "%02d" [expr $k+1]]_AXI]
}
#====================================
#Assign Address
#====================================
assign_bd_address
#Global Address
set timer					0x50000000
set uart					0x50010000
set groupIDGenerator		0x50020000
set groupNumberAllocator	0x50030000
set icap					0x50040000
set localCoreIDAllocator	0x50200000
set GSMem					0x50300000
set GlobalMem				0x80000000
set_property offset $timer [get_bd_addr_segs {Host/Data/SEG_axi_timer_0_Reg}]
set_property offset $uart [get_bd_addr_segs {Host/Data/SEG_axi_uartlite_0_Reg}]
set_property offset $icap [get_bd_addr_segs {Host/Data/SEG_axi_hwicap_0_Reg}]
set_property offset $groupIDGenerator [get_bd_addr_segs {Host/Data/SEG_groupIDGenerator_0_Reg}]
set_property offset $groupNumberAllocator [get_bd_addr_segs {Host/Data/SEG_groupNumberAllocator_0_Reg}]
set_property offset $GSMem [get_bd_addr_segs {Host/Data/SEG_GSMem_Mem0}]
set_property offset $GlobalMem [get_bd_addr_segs {Host/Data/SEG_GlobalMem_memaddr}]
for {set k 0} {$k < $numGroup} {incr k} \
{
	set_property offset "0x502[format "%01x" [expr $k]]0000" [get_bd_addr_segs "Host/Data/SEG_localCoreIDAllocator_$k\_Reg"]
}
assign_bd_address
if {$numACC!=0} \
{
	assign_bd_address [get_bd_addr_segs {GlobalMem/memmap/memaddr }]
}
#Slaves
for {set i 0} {$i < $numGroup} {incr i} \
{
	for {set j 0} {$j < $numGPP} {incr j} \
	{
		set_property offset $timer [get_bd_addr_segs "Slave$j\_Group$i/Data/SEG_axi_timer_0_Reg"]
		set_property offset $uart [get_bd_addr_segs "Slave$j\_Group$i/Data/SEG_axi_uartlite_0_Reg"]
		set_property offset $icap [get_bd_addr_segs "Slave$j\_Group$i/Data/SEG_axi_hwicap_0_Reg"]
		set_property offset $groupIDGenerator [get_bd_addr_segs "Slave$j\_Group$i/Data/SEG_groupIDGenerator_0_Reg"]
		set_property offset $groupNumberAllocator [get_bd_addr_segs "Slave$j\_Group$i/Data/SEG_groupNumberAllocator_0_Reg"]
		set_property offset $GSMem [get_bd_addr_segs "Slave$j\_Group$i/Data/SEG_GSMem_Mem0"]
		set_property offset $GlobalMem [get_bd_addr_segs "Slave$j\_Group$i/Data/SEG_GlobalMem_memaddr"]
		for {set k 0} {$k < $numGroup} {incr k} \
		{
			set_property offset "0x502[format "%01x" [expr $k]]0000" [get_bd_addr_segs "Slave$j\_Group$i/Data/SEG_localCoreIDAllocator_$k\_Reg"]
		}
	}
	for {set j 0} {$j < $numACC} {incr j} \
	{
		set_property offset $timer [get_bd_addr_segs "$typeACC$j\_Group$i/Data/SEG_axi_timer_0_Reg"]
		set_property offset $uart [get_bd_addr_segs "$typeACC$j\_Group$i/Data/SEG_axi_uartlite_0_Reg"]
		set_property offset $icap [get_bd_addr_segs "$typeACC$j\_Group$i/Data/SEG_axi_hwicap_0_Reg"]
		set_property offset $groupIDGenerator [get_bd_addr_segs "$typeACC$j\_Group$i/Data/SEG_groupIDGenerator_0_Reg"]
		set_property offset $groupNumberAllocator [get_bd_addr_segs "$typeACC$j\_Group$i/Data/SEG_groupNumberAllocator_0_Reg"]
		set_property offset $GSMem [get_bd_addr_segs "$typeACC$j\_Group$i/Data/SEG_GSMem_Mem0"]
		set_property offset $GlobalMem [get_bd_addr_segs "$typeACC$j\_Group$i/Data/SEG_GlobalMem_memaddr"]
		for {set k 0} {$k < $numGroup} {incr k} \
		{
			set_property offset "0x502[format "%01x" [expr $k]]0000" [get_bd_addr_segs "$typeACC$j\_Group$i/Data/SEG_localCoreIDAllocator_$k\_Reg"]
		}	
	}
}
#Generate Bitstream
#generate_target all [get_files  C:/Users/hding/Projects/DATE14/KC705_4GACC_Convolution0/KC705_4GACC_Convolution0.srcs/sources_1/bd/system/system.bd]
#make_wrapper -files [get_files C:/Users/hding/Projects/DATE14/KC705_4GACC_Convolution0/KC705_4GACC_Convolution0.srcs/sources_1/bd/system/system.bd] -top
#add_files -norecurse C:/Users/hding/Projects/DATE14/KC705_4GACC_Convolution0/KC705_4GACC_Convolution0.srcs/sources_1/bd/system/hdl/system_wrapper.v
#update_compile_order -fileset sources_1
#update_compile_order -fileset sim_1
#reset_run synth_1
#launch_runs impl_1 -to_step write_bitstream


