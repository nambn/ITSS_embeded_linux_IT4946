FILES := 8_2_pipe
all: $(FILES)

$(FILES) : $(FILES).c
	gcc -o $(FILES) $(FILES).c

clean:
	$(RM) $(FILES)