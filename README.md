# Malware Behavior Analyzer - MBA 
+ Continuous Integration (Travis-CI)  
[![Build Status](https://travis-ci.org/GlacierW/MBA.svg?branch=master)](https://travis-ci.org/GlacierW/MBA)

# Introduction
MBA is a QEMU-based, sandbox system dedicated to malware analysis for Windows x64
Currently, MBA supports the following features:

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

More features are under development.


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
