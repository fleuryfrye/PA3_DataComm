
BACKOFF := backoff
BACKOFF_EXECUTABLE := $(BACKOFF).exe


$(BACKOFF_EXECUTABLE): $(BACKOFF).o
	g++ $(BACKOFF).cpp -o $(BACKOFF_EXECUTABLE)


all: $(BACKOFF_EXECUTABLE)

clean:
	-rm -f *.exe