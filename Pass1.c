#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 25

void main()
{
    FILE *fp1,*fp2,*fp3,*fp4,*fp5,*fp6;
    char label[SIZE],opcode[SIZE],operand[SIZE],symbol[SIZE],address[SIZE],opvalue[SIZE],mnemonic[SIZE],error_desc[100];
    int start,length,size,locctr,op_found,error=0;
    fp1=fopen("input.txt","r");
    fp2=fopen("optab.txt","r");
    fp3=fopen("symtab.txt","w");
    fp4=fopen("intermediate.txt","w");
    fp5=fopen("length.txt","w");
    fp6=fopen("symtab.txt","r");

    fscanf(fp1,"%s\t%s\t%s",label,opcode,operand);
    if(strcmp(opcode,"START") == 0)
    {
        sscanf(operand,"%X",&start);
        locctr=start;
    }
    else
    {
        locctr=0;
    }
    fprintf(fp4,"*\t%s\t%s\t%s\n",label,opcode,operand);
    fscanf(fp1,"%s\t%s\t%s",label,opcode,operand);
    while(strcmp(opcode,"END") != 0 && error == 0)
    {
        fprintf(fp4,"%4X\t",locctr);
        if(strcmp(label,"-") != 0)
        {
            if(strcmp(label,"*") != 0)
            {
                while(fscanf(fp6,"%s\t%s\t",symbol,address) != EOF)
                {
                    if(strcmp(label,symbol) == 0)
                    {
                        error=1;
                        strcat(error_desc,"ERROR : DUPLICATE SYMBOL ");
                        strcat(error_desc,label);
                        break;
                    }
                }
                rewind(fp6);
                if(!error)
                {
                    fprintf(fp3,"%s\t%X\t\n",label,locctr);
                     fflush(fp3);
                }
            }
            if(strcmp(opcode,"WORD") == 0)
            {
                locctr+=3;
            }
            else if(strcmp(opcode,"RESW") == 0)
            {
                locctr+=3*atoi(operand);
            }
            else if(strcmp(opcode,"RESB") == 0)
            {
                locctr+=atoi(operand);
            }
            else if(strcmp(opcode,"BYTE") == 0)
            {
                locctr+=strlen(operand)-3;
            }
            else
            {
                while(fscanf(fp2,"%s\t%s",opvalue,mnemonic) != EOF)
                {
                    op_found=0;
                    if(strcmp(opcode,opvalue) == 0)
                    {
                        op_found=1;
                        break;
                    }
                }
                rewind(fp2);
                if(op_found)
                {
                    locctr+=3;
                }
                else
                {
                    error=1;
                    strcat(error_desc,"ERROR : OPCODE NOT FOUND ");
                    strcat(error_desc,opcode);
                }
            }
        }
        fprintf(fp4,"%s\t%s\t%s\n",label,opcode,operand);
        fscanf(fp1,"%s\t%s\t%s",label,opcode,operand);
    }
    if(error)
    {
        printf("%s\n",error_desc);
    }
    else
    {
        length=locctr-start;
        size=locctr-start;
        fprintf(fp4,"*\t*\t%s\t*\n",opcode);
        printf("ASSEMBLED SUCCESSFULLY\n");
        printf("Length of the program : %d\n",length);
        fprintf(fp5,"Length=%d\nLength in hexa=%X",length,size);
    }

    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    fclose(fp4);
    fclose(fp5);
    fclose(fp6);
}
