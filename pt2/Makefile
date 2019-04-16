LIBS =
HEADERS =
OBJECTS1 = cs_2_server.o
OBJECTS2 = cs_2_client.o
FLAGS =  -O3 -g

default: all

%.o: %.c $(HEADERS)
	gcc -w $(FLAGS) -c $< -o $@

all: cs_2_server cs_2_client

server: $(OBJECTS1)
	gcc -w $(FLAGS) $(OBJECTS1) $(LIBS) -o $@

client: $(OBJECTS2)
	gcc -w $(FLAGS) $(OBJECTS2) $(LIBS) -o $@

clean:
	 -rm -f $(OBJECTS1)
	 -rm -f $(OBJECTS2)
	 -rm -f cs_2_server
	 -rm -f cs_2_client
