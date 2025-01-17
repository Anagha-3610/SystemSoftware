#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define tsize 10
#define SIZE 50

void main()
{
    FILE *fp1,*fp2,*fp3,*fp4,*fp5,*fp6;
    char label[SIZE],opcode[SIZE],operand[SIZE],locctr[SIZE],opvalue[SIZE],mnemonic[SIZE],symbol[SIZE],address[SIZE],obj_code[SIZE],error_desc[SIZE];
    int start,op_found,sym_found,error=0,operand_value,t_length=0,obj_generated;
    char size[SIZE],length[SIZE];

    fp1=fopen("intermediate.txt","r");
    fp2=fopen("optab.txt","r");
    fp3=fopen("symtab.txt","r");
    fp4=fopen("length.txt","r");
    fp5=fopen("output.txt","w");
    fp6=fopen("object_program.txt","w");

    fscanf(fp1,"%s\t%s\t%s\t%s",locctr,label,opcode,operand);
    //fscanf(fp4,"%s\t%s\t%s",length,size);
    if(strcmp(opcode,"START") == 0)
    {
        fprintf(fp5,"\t%s\t%s\t%s\n",label,opcode,operand);
        sscanf(operand,"%d",&start);
        fprintf(fp6,"H^%-6s^00%d^0000%s\n",label,start,size);
        fprintf(fp6,"T^00%d^00",start);
    }

    fscanf(fp1,"%s\t%s\t%s\t%s",locctr,label,opcode,operand);
    while(strcmp(opcode,"END") != 0 && error == 0)
    {
        obj_generated=0;
        if(strcmp(label,"-") != 0)
        {
            op_found=0;
            while(fscanf(fp2,"%s\t%s",opvalue,mnemonic) != EOF)
            {
                if(strcmp(opcode,opvalue) == 0)
                {
                    op_found=1;
                    strcat(obj_code,mnemonic);
                    if(isalpha(operand[0]))
                    {
                        sym_found=0;
                        while(fscanf(fp3,"%s\t%s\t",symbol,address) != EOF)
                        {
                            if(strcmp(symbol,operand) == 0)
                            {
                                sym_found=1;
                                strcat(obj_code,address);
                                obj_generated=1;
                                break;
                            }
                        }
                        if(sym_found == 0)
                        {
                            error=1;
                            strcat(error_desc,"ERROR : UNDEFINED SYMBOL");
                            strcat(error_desc,operand);
                        }
                        rewind(fp3);
                    }
                    break;
                }
            }
            rewind(fp2);
            if (op_found == 0)
            {
                if (strcmp(opcode, "WORD") == 0)
                {
                    // Convert operand to integer and append it to obj_code as a hexadecimal string
                       sscanf(operand, "%d", &operand_value); // Assuming WORD is in decimal format
                       char operand_str[10];
                       sprintf(operand_str, "%06X", operand_value);  // Convert to a 6-digit hexadecimal string
                       strcat(obj_code, operand_str);  // Concatenate operand as a string
                       obj_generated=1;
                }
                else if (strcmp(opcode, "BYTE") == 0)
                {
                    if (operand[0] == 'X')
                    {
                        // Handle BYTE with hexadecimal value
                        strncpy(obj_code, &operand[2], strlen(operand) - 3);  // Skip X'...'
                        obj_generated=1;
                    }
                    else if (operand[0] == 'C')
                    {
                        // Handle BYTE with character value
                        for (int i = 2; i < strlen(operand) - 1; i++)
                        {
                            char byte_val[3];
                            sprintf(byte_val, "%02X", operand[i]);  // Convert each char to hex
                            strcat(obj_code, byte_val);
                            obj_generated=1;
                        }
                    }
                }
            }
        }
        if(obj_generated)
        {
            fprintf(fp6,"^%s",obj_code);
            t_length+=3;
        }
        fprintf(fp5,"%s\t%s\t%s\t%s\t%s\n",locctr,label,opcode,operand,obj_code);
        strcpy(obj_code,"");
        fscanf(fp1,"%s\t%s\t%s\t%s",locctr,label,opcode,operand);
    }

    if(error == 1)
    {
        printf("%s\n",error_desc);
    }
    else
    {
        fseek(fp6,30,SEEK_SET);
        fprintf(fp6,"%-2X",t_length);
        fseek(fp6,0,SEEK_END);
        fprintf(fp6,"\nE^00%d\n",start);
        fprintf(fp5,"\t\t%s\n",opcode);
        printf("ASSEMBLED SUCCESSFULLY\n");
    }

    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    fclose(fp4);
    fclose(fp5);
    fclose(fp6);
}
