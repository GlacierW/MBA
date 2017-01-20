# Malware Behavior Analyzer - MBA 
+ Continuous Integration (Travis-CI)  
[![Build Status](https://travis-ci.org/GlacierW/MBA.svg?branch=master)](https://travis-ci.org/GlacierW/MBA)

# Introduction
MBA is a QEMU-based, sandbox system dedicated to malware analysis.  
Currently, MBA is mainly for the x86_64 architecture and Win10 x64 guest OS.  
The following features are supported:

    1. De-coupled Information Flow Tracking (DIFT)
       It is also known as taint analysis.
       Leveraging the DIFT feature, the contamination 
       conducted by malware to the guest OS can be identified.

       We would like to thank Chi-Wei, Wang, who developed the 
       initial version of DIFT, for sharing his source code. The
       original DIFT is mainly for x86 platform. In this project,
       the DIFT is upgraded to support modern x86_64 platform.

       For the academic paper published by Chi-Wei, Wang, please cite
       C. W. Wang and S. P. Shieh, "SWIFT: Decoupled System-Wide 
       Information Flow Tracking and its Optimizations," Journal of 
       Information Science and Engineering, JISE, 2015.
       http://www.iis.sinica.edu.tw/page/jise/2015/201507_15.pdf

    2. Disk Forensics (Tsk)
       The tsk extension supports the translation between the
       disk sector address and the corresponding file in the
       Windows guest OS.

    3. Memory Forensics (MemFrs)
       The memfrs extension performs the virtual machine introspection
       on the guest physical/virtual memory. The high-level semantics
       of the memory data (e.g. process list) can be interpreted without
       the assistence of the guest OS. 

    4. Windows in-VM Agent (Agent)
       The agent extension provides a convenient communication between 
       the QEMU console and the guest OS. An agent program is needed 
       to be pre-installed inside the Windows guest to support the 
       operations such as ...
         > execute a command in the guest w/wo return output expected
         > import/export a file into/from the guest

    5. Out-of-Box Hooking (OBHook)
       The obhook extension provides the VM-based hook against the guest OS.
       This features allows MBA to intercept the event-of-interest of the guest.
       Note that the obhook is purely implemented beneath the guest OS, namely
       the hypervisor (QEMU). Thereby, not a code snippets is required to be
       inserted to the guest, and thus prevent the interference of malware.

    6. Network Traffic Monitor (NetTraMon)
       The nettramon extension performs monitoring the network traffic of the guest OS.
       This extension provides sniffing, parsing, and filtering packets, and supports protrocols of TCP, UDP and ICMP.
       User can also set files for storing parsed packets according to protocols.

More features are under development.

# Quick Start
Download MBA source 

    $ git clone https://github.com/GlacierW/MBA
    $ cd MBA/

Configure QEMU to enable all of the MBA features.  
To enable a MBA feature individually, please refer to `./configure -h`

    $ ./configure --target-list=x86_64-softmmu --enable-mba-all
    
Compile MBA. (May take a while, use `-j<CPU_CORE>` to speedup)

    $ make

Start MBA with the prepared Win10_64bit image in QCOW2 format.  
Due to the implementation issue, 2048MB RAM and 16GB disk space for the guest OS are recommended.  

    $ ./x86_64-softmmu/qemu-system-x86_64 \
      -m 2048 \
      -hda <YOUR_IMAGE> \
      -net nic,model=rtl8139 -net user \
      -k en-us -usb \
      -monitor stdio \
      -vnc :1 

Now the VNC server should be able to connect to via the port 5901.  

The APIs of each MBA extension can be found under the `ext` directory.  
The documents are under preparation.

# Members
Chi-Wei, Wang       cwwangabc@gmail.com  
Chia-Wei, Wang      chiaweiw@cs.nctu.edu.tw  
Chong-Kuan, Chen    ckchen@cs.nctu.edu.tw  
Hao, Li             misterlihao@gmail.com  
Jui-Chien, Jao      jcjao0120@gmail.com  
Chuan-Hua, Cheng    newchengxx@gmail.com  
E-Lin, Ho           dennisieur210@gmail.com  

Network Security Lab,  
National Chiao-Tung University, Taiwan
