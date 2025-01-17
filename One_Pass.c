#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SIZE 50

void main() {
    FILE *fp1, *fp2, *fp3;
    char label[SIZE], opcode[SIZE], operand[SIZE], locctr_str[SIZE], opvalue[SIZE], mnemonic[SIZE], symbol[SIZE], address[SIZE];
    char obj_code[SIZE], error_desc[200], length_str[SIZE];
    int start, locctr, op_found, sym_found, operand_value, error = 0, t_length = 0, obj_generated;
    
    fp1 = fopen("input.txt", "r");
    fp2 = fopen("optab.txt", "r");
    fp3 = fopen("output.txt", "w");

    fscanf(fp1, "%s\t%s\t%s", label, opcode, operand);

    if (strcmp(opcode, "START") == 0) {
        sscanf(operand, "%X", &start);
        locctr = start;
        fprintf(fp3, "H^%-6s^00%X^XXXX\n", label, start);  // Placeholder for length
        fprintf(fp3, "T^00%X^00", start);  // Placeholder for text record length
    } else {
        locctr = 0;
    }

    fscanf(fp1, "%s\t%s\t%s", label, opcode, operand);
    while (strcmp(opcode, "END") != 0 && error == 0) {
        obj_generated = 0;
        
        if (strcmp(label, "-") != 0) {
            op_found = 0;

            // Search for opcode in OPTAB
            while (fscanf(fp2, "%s\t%s", opvalue, mnemonic) != EOF) {
                if (strcmp(opcode, opvalue) == 0) {
                    op_found = 1;
                    strcpy(obj_code, mnemonic);
                    if (isalpha(operand[0])) {
                        // Handle symbolic operand
                        sym_found = 0;
                        FILE *symtab = fopen("symtab.txt", "r");
                        while (fscanf(symtab, "%s\t%s", symbol, address) != EOF) {
                            if (strcmp(symbol, operand) == 0) {
                                sym_found = 1;
                                strcat(obj_code, address);
                                obj_generated = 1;
                                break;
                            }
                        }
                        fclose(symtab);
                        if (sym_found == 0) {
                            error = 1;
                            strcat(error_desc, "ERROR: Undefined Symbol");
			    strcat(error_desc,operand);
                        }
                    }
                    break;
                }
            }
            rewind(fp2);

            if (op_found == 0) {
                if (strcmp(opcode, "WORD") == 0) {
                    sscanf(operand, "%d", &operand_value);
                    sprintf(obj_code, "%06X", operand_value);
                    obj_generated = 1;
                } else if (strcmp(opcode, "BYTE") == 0) {
                    if (operand[0] == 'X') {
                        strncpy(obj_code, &operand[2], strlen(operand) - 3);
                        obj_generated = 1;
                    } else if (operand[0] == 'C') {
                        for (int i = 2; i < strlen(operand) - 1; i++) {
                            char byte_val[3];
                            sprintf(byte_val, "%02X", operand[i]);
                            strcat(obj_code, byte_val);
                            obj_generated = 1;
                        }
                    }
                } else {
                    error = 1;
                    strcat(error_desc, "ERROR: Invalid Opcode");
		    strcat(error_desc,opcode);
                }
            }
        }

        if (obj_generated) {
            fprintf(fp3, "^%s", obj_code);
            t_length += 3;
        }

        // Update location counter based on the opcode type
        if (strcmp(opcode, "WORD") == 0) {
            locctr += 3;
        } else if (strcmp(opcode, "RESW") == 0) {
            locctr += 3 * atoi(operand);
        } else if (strcmp(opcode, "RESB") == 0) {
            locctr += atoi(operand);
        } else if (strcmp(opcode, "BYTE") == 0) {
            locctr += strlen(operand) - 3;
        } else if (op_found) {
            locctr += 3;
        }

        // Read next instruction
        fscanf(fp1, "%s\t%s\t%s", label, opcode, operand);
    }

    if (error) {
        printf("%s\n", error_desc);
    } else {
        fseek(fp3, 14, SEEK_SET);
        fprintf(fp3, "%02X", t_length);
        fseek(fp3, 0, SEEK_END);
        fprintf(fp3, "\nE^00%X\n", start);
        printf("ASSEMBLED SUCCESSFULLY\n");
    }

    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
}
