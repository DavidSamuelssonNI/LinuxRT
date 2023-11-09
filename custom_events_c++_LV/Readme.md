Install opendds  
source setenv.sh from its root  

in example  
/home/admin/OpenDDS/DevGuideExamples/DCPS/Messenger

build with cmake .. && make from build directory

TEST:
Use 
Send memory -> SemaphoreReciever.cpp (This waits for passLVRedToSharedMem to ask for memory)
Receiving memory -> testlv.cpp to trigger passLVRefToSharedMem.cpp to run

Real:
Start -> publisher and subscriber
subscriber will wait for SemaphoreReciever.cpp to ask for the recieved signal

explore_events3. vi Labview will trigger the SemaphoreReciever.cpp (so file). This puts the recieved memory back to LV

Other example:
mylilb.cpp + explore_events.vi


