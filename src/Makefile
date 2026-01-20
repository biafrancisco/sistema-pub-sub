CFLAGS = -Wall 

objs_sub = utils.o logger.o subscriber.o 
objs_pub = utils.o logger.o publisher.o 

all: subscriber publisher
 
# regras de ligacao
subscriber: $(objs_sub) 
	gcc $(CFLAGS) $(objs_sub) -o subscriber

publisher: $(objs_pub) 
	gcc $(CFLAGS) $(objs_pub) -o publisher
 
# regras de compilação
utils.o: utils.c utils.h
logger.o: logger.c logger.h
subscriber.o: subscriber.c
publisher.o: publisher.c 


# remove arquivos temporários
clean:
	-rm -f $(objs_sub) *~
	-rm -f $(objs_pub) *~
 
# remove tudo o que não for o código-fonte
purge: clean
	-rm -f subscriber
	-rm -f publisher


