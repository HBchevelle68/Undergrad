#define UPDATEALL 0x55
#define CODE1 0x43
#define CODE2 0x51
//#define CODE3 0x55
#define CODE4 0x57
#define CODE5 0x45
#define CODE6 0x56
#define CODE7 0x01
#define CODE8 0x00

//Command Protocol Standard Ordering: ONLY HANDLES ONE TARGET AS OF NOW
//	[Action] [Target] [Specific/Rand] [value/or nothing]

int check_valid(char test, int n[]){
	for(int i = 0; i < sizeof(n); i++){
		if((int)test == n[i]){
			return 5;
		}
	}
	return 0;
}
int process(unsigned char* code){
	int valid_targets[] = {CODE4, CODE5, CODE6};
	if((code[0] & 0xFF) == UPDATEALL){
		printf("UPDATE ALL\n");
		return 0;
	}
	if(check_valid(code[1], valid_targets) == 0){
		puts("Invalid target");
		return 5;
	}
	else if((int)code[2] != CODE7 && (int)code[2] != CODE8){
		puts("Specific or Random Option not found");
		return 5;
	}
	switch(code[0] & 0xFF){
		case CODE1:
			printf("Valid Code\n");
			return 0;
		case CODE2:
			printf("Valid Code\n");
			return 0;
		default:
			printf("Invalid Command, %d\n", code[0]);
	}
}
