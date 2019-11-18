CC=g++ -I. -D_THREAD_SAFE -DAIX  -g   -Wall  
NAME=filecollection_audit_st1
objects=ssh.o sftp.o frame_memory.o frame_mutex.o exception.o frame_utils.o thread.o myatomic_1.o definitions.o dbprocess.o g_string.o memmanager.o main.o fileprocess.o logprocess.o iniprocess.o schedule.o ftpprocess.o taskmanager.o regexpress.o taskitem.o

$(NAME): $(objects)
	$(CC)  -o $(NAME) -L$(ORACLE_HOME)/lib  -lssh2 -lm -lrt -lnsl -lpthread -lclntsh  -lstdc++ $(objects)

#/ricalibs/tools/gcc-3.3.2/powerpc-ibm-aix5.3.0.0/pthread/libstdc++-v3/src/.libs/libstdc++.a
ssh.o: ssh.h
sftp.o: ssh.h sftp.h
g_string.o: g_string.h
memmanager.o: memmanager.h g_string.h
fileprocess.o: fileprocess.h
logprocess.o: logprocess.h
ftpprocess.o: logprocess.h
regexpress.o: regexpress.h
iniprocess.o: iniprocess.h g_string.h
schedule.o: schedule.h logprocess.h
taskitem.o: iniprocess.h ftpprocess.h memmanager.h
dbprocess.o: dbprocess.c dbprocess.h
frame_memory.o: frame_memory.h
frame_mutex.o: frame_mutex.h
exception.o: exception.h
frame_utils.o: frame_utils.h
thread.o: thread.h
myatomic_1.o: myatomic_1.h
definitions.o: definitions.h
dbprocess.c: dbprocess.pc 
	proc code=cpp  parse=none line=yes iname=$^ oname=$@ 
taskmamanger.o:schedule.h iniprocess.h memmanager.h regexpress.h ftpprocess.h fileprocess.h taskitem.h sftp.h
main.o: frame_memory.o frame_mutex.o exception.o frame_utils.o thread.o myatomic_1.o definitions.o dbprocess.o ssh.o sftp.o g_string.o fileprocess.o logprocess.o iniprocess.o schedule.o ftpprocess.o taskmanager.o taskitem.o

install:$(NAME)
	cp -f $(NAME) ../$(NAME)

clean:
	rm -f *.o
	rm -f $(NAME)
	rm -f ../$(NAME)
	rm -f dbprocess.c

