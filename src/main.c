#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "graph.h"
#include "dfs_stack.h"

//Validate the input arguments, bullet proof the program
int main(int argc, char *argv[])
{

	//$./mymake Makefile This fork and exec the makefile form start to finish
	//Similarly account for -r flag
	if (argc == 2 && strncmp(argv[1], "-p", 2)) {

		if(process_file(argv[1])==-1){ //Error Checking to see if the program Failed to open
			exit(0);
		}
		target blocks[128];
		int count= parsemakefile(blocks); //parsing lines of the Makefile update
		for(int i; i<=count; i++){
			for(int j=0;j<blocks[i].dep_count;j++)
				strtok(blocks[i].depend[j],"\n");
			for(int k=0;k<blocks[i].recipe_count;k++)
				strtok(blocks[i].recipe[k],"\n");
		}

		pid_t pid;
		stack_t *stack = createStack();
		push(stack,blocks[0].name); //pushing the first target on the stack
		while(stack->size!=0){ //DFS
			target curblock=findblock(stack,count,blocks);//Get the target struct from top of stack
			target dep[curblock.dep_count];
			target* deps=getdep(curblock, count,blocks,dep);
			for(int i=0;i<curblock.dep_count;i++){
				if(deps[i].visited==1 || count==0){
					if(i==(curblock.dep_count-1)){
					pop(stack);
					for(int j=0;j<curblock.recipe_count;j++){
					pid=fork();
					if(pid==0){
						char* recipe=curblock.recipe[j];
						char** temp;
						temp=malloc(sizeof(char *)*128);
						for (int k = 0; k < 128; k++) {
							temp[k]= malloc(sizeof(char)*25);
						}
						recipe=strtok(recipe," ");// finds spaces to parse the recipe
						int c=0;
						while(recipe!=NULL){//exits at end of recipe
							temp[c]=recipe;
							recipe=strtok(NULL," ");
							c++;
						}
						temp[c]=NULL;//add a NULL at the end of array for execvp
						execvp(temp[0],temp);
					}
				}
				wait(NULL);
					curblock.visited=1;
					updateblock(count, blocks, curblock);//update the Blocks Array
				}
				}else{
					push(stack,deps[i].name);
					break;
				}
			}
			if(curblock.dep_count==0){//Check to see if target has depends
				pop(stack);
				for(int j=0;j<curblock.recipe_count;j++){
				pid=fork();
				if(pid==0){
					char* recipe=curblock.recipe[j];
					char** temp;
					temp=malloc(sizeof(char *)*128);
					for (int k = 0; k < 128; k++) {
						temp[k]= malloc(sizeof(char)*25);
					}
					recipe=strtok(recipe," ");// finds spaces to parse the recipe
					int c=0;
					while(recipe!=NULL){
						temp[c]=recipe;
						recipe=strtok(NULL," ");
						c++;
					}
					temp[c]=NULL;//add a NULL at the end of array for execvp
					execvp(temp[0],temp);
				}
			}
			  wait(NULL);//wait for child process
				curblock.visited=1;
				updateblock(count, blocks, curblock);
			}

		}

	}
	if (argc == 3) {
//$./mymake -r Makefile This prints the recipes in order on the screen
	if(strcmp(argv[1],"-r")==0){
		if (process_file(argv[2])) { //Error checking
			fprintf(stderr, "Exiting, %s was not open\n",argv[2]);
			exit(0);
		}
		process_file(argv[2]);
		target blocks[128]; //Create array of targets
		int count= parsemakefile(blocks); //parse makefile
		for(int i; i<=count; i++){
			for(int j=0;j<blocks[i].dep_count;j++)
			strtok(blocks[i].depend[j],"\n");
			for(int k=0;k<blocks[i].recipe_count;k++)
			strtok(blocks[i].recipe[k],"\n");
		}

	stack_t *stack = createStack();
	push(stack,blocks[0].name);
	while(stack->size!=0){
		target curblock=findblock(stack,count,blocks);
		target dep[curblock.dep_count];
		target* deps=getdep(curblock, count,blocks,dep);
		for(int i=0;i<curblock.dep_count;i++){
			if(deps[i].visited==1 || count==0){
				if(i==(curblock.dep_count-1)){
				pop(stack);
				for(int j=0;j<curblock.recipe_count;j++)
				printf("%s\n",curblock.recipe[j]);
				curblock.visited=1;
				updateblock(count, blocks, curblock); //update the blocks array
			}
			}else{
				push(stack,deps[i].name);
				break;
			}
		}
		if(curblock.dep_count==0){ //Check to see if there is only one depend
			pop(stack);
			for(int j=0;j<curblock.recipe_count;j++)
			printf("%s\n",curblock.recipe[j]);
			curblock.visited=1;
			updateblock(count, blocks, curblock); //update the blocks array
		}

	}
	}else if (!strncmp(argv[1], "-p", 2)) {
			//$./mymake -p Makefile print the recipe, and depends in order
			if (process_file(argv[2])) {//Error check for the file
				fprintf(stderr, "Exiting %s\n",argv[2]);
				exit(0);
			}
			process_file(argv[2]);
			target blocks[128];
			int count= parsemakefile(blocks);

		for(int i; i<=count; i++){
		printf("target '%s' has %d dependencies and %d recipe\n",blocks[i].name,blocks[i].dep_count,blocks[i].recipe_count);
		for(int j=0;j<blocks[i].dep_count;j++){
		strtok(blocks[i].depend[j],"\n");
		printf("Dependency %d is %s\n",j,blocks[i].depend[j]);
	}
		for(int k=0;k<blocks[i].recipe_count;k++){
		strtok(blocks[i].recipe[k],"\n");
		printf("Recipe %d is %s\n",k,blocks[i].recipe[k]);
		}
		printf("\n");
	}

		}
	}

	exit(EXIT_SUCCESS);
}
//Parse the input makefile to determine targets, dependencies, and recipes
int process_file(char *fname)
{
	FILE* fp = fopen(fname, "r");
	char line[LINE_SIZE];
	int i = 0;

	if (!fp) {
		printf("Failed to open the file: %s \n", fname);
		return -1;
	}

	//Read the contents and store in lines
	while (fgets(line, LINE_SIZE, fp))
		strncpy(lines[i++], line, strlen(line));

	fclose(fp);

	return 0;
}

int parsemakefile(target *blocks){//Parse the Makefile using strtok, and strchr
	char *result;
	char *newline="\n";
	int count = 0;
	int k=0;
	int arrlen =sizeof(lines)/sizeof(lines[0]);
	for(int i= 0; i < arrlen; i++) {
		if(strchr(lines[i],':')!=NULL){
			if(i!=0){
				k=0;
				count++;
			}
			result=strtok(lines[i]," :");
			blocks[count].name=result;
			int j=0;
			while( result != NULL) {
				result = strtok(NULL," :");
				 if(result != NULL){
					 if(strcmp(result,newline)!=0){
					 blocks[count].depend[j]=result;
					 j++;
				 }
				 }
			 }
			 blocks[count].dep_count=j;
		}else{
			if(strchr(lines[i],' ')!=NULL){
				result=strtok(lines[i],"\t");
				blocks[count].recipe[k]=result;
				k++;
			}
			blocks[count].recipe_count=k;
		}
	}
	return count;
}

stack_t *createStack(void){//Creates a new stack
	stack_t *stack= malloc(sizeof(stack_t));
	stack->targets=malloc(sizeof(char *)*128);
	for (int i = 0; i < 128; i++) {
		stack->targets[i]= malloc(sizeof(char)*25);
  }
	stack->size=0;
	return stack;
}

void push(stack_t* stack, char* item){
	if(stack->size==128){
		perror("Stack is full\n");
		exit(0);
	}
	strcpy((stack->targets)[stack->size++], item);
}

char* peek(stack_t* stack){
	if(stack->size==0){
	fprintf(stderr,"stack is empty\n");
	}
	int size = stack->size;
	return stack->targets[size-1];
}

char* pop(stack_t* stack) {
	if(stack->size==0){
	fprintf(stderr,"stack is empty\n");
	return NULL;
  }
	return *(stack->targets + (--stack->size));
}

target findblock(stack_t* stack, int count,target* blocks){
	char* top =peek(stack);
	for(int i=0; i <= count; i++){
		if(strcmp(blocks[i].name,top)==0){
			return blocks[i];
		}
	}
	exit(0);
}
target* getdep(target block, int count, target* blocks, target* dep){//Get the depends of a block, and return array of block
	for(int i=0;i<=count;i++){
		for(int j=0;j<block.dep_count;j++){
		if(strcmp(blocks[i].name,block.depend[j])==0){
			dep[j]=blocks[i];
		}
	}
}
	return dep;
}

void updateblock(int count, target* blocks,target block){//update blocks array
	char* blockname=block.name;
	for(int i=0; i <= count; i++){
		if(strcmp(blocks[i].name,blockname)==0){
			blocks[i]=block;
		}
	}
}
