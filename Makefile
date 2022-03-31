CC=gcc
CFLAGS=-I/usr/include/postgresql
LDDIR=-L/usr/lib
LDFLAGS=-lpq
BINS=update_postgressql delete_repoip eth_hotplug random_string


all: $(BINS)

$(BINS): % : %.c
	$(CC) $(CFLAGS) $< $(LDDIR) $(LDFLAGS) -o $@

clean:
	rm -rf *.o $(BINS) $(MBINS)
	rm -rf *~
