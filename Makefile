NAME	= report2

all: $(NAME).mb

crt.o: crt.S
	arm-thumb-elf-as -o crt.o crt.S

$(NAME).s: $(NAME).c
	arm-thumb-elf-gcc -S $(NAME).c

$(NAME).o: $(NAME).s
	arm-thumb-elf-as -o $(NAME).o $(NAME).s

$(NAME).out: $(NAME).o crt.o
	arm-thumb-elf-ld -T gcc.ls -o $(NAME).out crt.o $(NAME).o

$(NAME).mb: $(NAME).out
	arm-thumb-elf-objcopy -O binary $(NAME).out $(NAME).mb

clean:
	rm -rf $(NAME).o $(NAME).s $(NAME).out $(NAME).mb crt.o

