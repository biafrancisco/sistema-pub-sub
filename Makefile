CFLAGS = -Wall 

objs_sub = ./src/utils.o ./src/logger.o ./src/subscriber.o 
objs_pub = ./src/utils.o ./src/logger.o ./src/publisher.o 

all: ./src/subscriber ./src/publisher
 
# regras de ligacao
./src/subscriber: $(objs_sub) 
	gcc $(CFLAGS) $(objs_sub) -o subscriber

./src/publisher: $(objs_pub) 
	gcc $(CFLAGS) $(objs_pub) -o publisher
 
# regras de compilação
./src/utils.o: ./src/utils.c ./src/utils.h
./src/logger.o: ./src/logger.c ./src/logger.h
./src/subscriber.o: ./src/subscriber.c
./src/publisher.o: ./src/publisher.c 


# remove arquivos temporários
clean:
	-rm -f $(objs_sub) *./src/~
	-rm -f $(objs_pub) *./src/~
 
# remove tudo o que não for o código-fonte
purge: clean
	-rm -f subscriber
	-rm -f publisher


