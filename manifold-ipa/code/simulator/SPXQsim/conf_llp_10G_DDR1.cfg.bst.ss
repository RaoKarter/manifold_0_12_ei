//simulation_stop = 700000000000L; //
simulation_stop = 500000000L; //

network_clock_frequency = 3000000000L; // 1GHz

distributed_clock_frequency = [1e9, 1e9, 1e9, 1e9, 
                               1e9, 1e9, 1e9, 1e9,
                               1e9, 1e9, 1e9, 1e9,
                               1e9, 1e9, 1e9, 1e9];

cache_clock_frequency = [1e9, 1e9, 1e9, 1e9,
			 1e9, 1e9, 1e9, 1e9,
			 1e9, 1e9, 1e9, 1e9,
			 1e9, 1e9, 1e9, 1e9];

core_thermal_threshold = [340, 340, 340, 340,
                          340, 340, 340, 340,
                          340, 340, 340, 340,
                          340, 340, 340, 340];

// V = 0.8 + 0.1(f - 3e9)/1e9
core_voltage = [0.6, 0.6, 0.6, 0.6,
                0.6, 0.6, 0.6, 0.6,
                0.6, 0.6, 0.6, 0.6,
                0.6, 0.6, 0.6, 0.6]; 

sampling_period = 1e-3;

network:
{
    topology = "TORUS6P";
    x_dimension = 4;
    y_dimension = 4;
    num_vcs = 4;
    credits = 32;
    link_width = 512;

    ni_up_credits = 64; //credits for network interface sending to terminal
    ni_up_buffer = 32; //network interface's output buffer (to terminal) size

    coh_msg_type = 123; //message types
    mem_msg_type = 456;
    credit_msg_type = 789;
};

processor:	
{
    node_idx = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];
};

llp_cache:
{
    name = "L1";
    type = "DATA";
    size = 0x4000; //16K
    assoc = 8;
    block_size = 64;
    hit_time = 1;
    lookup_time = 1;
    replacement_policy = "LRU";
    mshr_size = 32;

    downstream_credits = 32; //credits for sending to network
};

lls_cache:
{
    name = "L2";
    type = "DATA";
    size = 0x200000; 	//2MB
    assoc = 8;		//8
    block_size = 64;
    hit_time = 24;	//24
    lookup_time = 24;	//24
    replacement_policy = "LRU";
    mshr_size = 128;	//128

    downstream_credits = 128; //credits for sending to network
};

mc: //memory controller
{ 
    //node_idx = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];
    node_idx = [3, 6, 9, 12];
    downstream_credits = 128; //credits for sending to network
    //type = "CAFFDRAM";
    type = "DRAMSIM";
    dram_clock_frequency = 800000000;	// 800 MHz
    dramsim2:
    {
        //dev_file = "3d_die_4GB.ini";
        dev_file = "3d_die_DDR1.ini";
        sys_file = "3d_die.ini.example";
        size = 1024;
        //size = 256;
        freq_scaling_option = 1;
    };
};
