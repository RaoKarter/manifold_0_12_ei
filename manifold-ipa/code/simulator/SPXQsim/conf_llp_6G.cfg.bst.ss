//simulation_stop = 700000000000L; //
simulation_stop = 1000000000L; //

network_clock_frequency = 3000000000L; // 3GHz

distributed_clock_frequency = [0.6e9, 0.6e9, 0.6e9, 0.6e9,
                               0.6e9, 0.6e9, 0.6e9, 0.6e9,
                               0.6e9, 0.6e9, 0.6e9, 0.6e9,
                               0.6e9, 0.6e9, 0.6e9, 0.6e9];

core_voltage = 0.5163; // V = (1.93e-4 * f) + 0.4008

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
    size = 0x200000; //2MB
    assoc = 64;
    block_size = 64;
    hit_time = 24;
    lookup_time = 24;
    replacement_policy = "LRU";
    mshr_size = 128;

    downstream_credits = 128; //credits for sending to network
};

mc: //memory controller
{ 
    node_idx = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];
    downstream_credits = 128; //credits for sending to network
    type = "DRAMSIM";
    dramsim2:
    {
        dev_file = "3d_die.ini";
        sys_file = "3d_die.ini.example";
        size = 256;
   };
};
