Part 1
Group members:

Name            section numbers Emails
Zhi Hong        section 2-13435 hua@csu.fullerton.edu
Joshua Womack   section 2-13435 jdwomack@csu.fullerton.edu
Yijie Sun       section 2-13435 maplesyj@gmail.com
Ruth Obidah     section 3-13500 ruthobidah@csu.fullerton.edu

Part 2
How to run program.
1. for the message queue and shared memory.
    there are 6 files.
      sender.c      send file program
      recv.c        receive file program   
      msg.h         head file for message queue.
      keyfile.txt   use to generate id.
      sender.txt    send text file
      recvfile.txt  receive text file
how to run:
    open two terminals in linux
        In one terminal type:   g++ -o sender sender.c && ./sender  sender.txt
        In other terminal type: g++ -o recv recv.c && ./recv
    
2. for the signal and shared memory.
    there are 5 files.
      sender.c      send file program
      recv.c        receive file program   
      keyfile.txt   use to generate id.
      sender.txt    send text file
      recvfile.txt  receive text file
how to run:
    open two terminals in linux
        In other terminal type: g++ -o recv recv.c && ./recv    
        In one terminal type:   g++ -o sender sender.c && ./sender  sender.txt

Part 3
We gathered together two hours before the 351 class, and discussed the design, implementation and other related topics.
Zhi Hong: did the debug job, contributed to some of the design, and put everyone's work together. 
Joshua Womack: worked on design and debugging. 
Yijie Sun: discussed the design, and worked on implementation. 
Ruth Obidah: worked on design and scheduling meetings, and made sure that we were on track with our schedule.

