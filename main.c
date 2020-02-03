/**

Hello.
I compress the string by letting A=00, T=01, C=10, G=11.
Next I add the consecutive four A,T,C,G together with the use of bitwise operation << and yield a number lying in [0,255].
Then I represent that sum by the unsigned character under the ASCII table.
There may be remaining term in the string, I still add them together, whichever comes first is put on leftmost by bitwise operation << .
If the string consists of character other than A,T,C,G, it will not be compressed.

More importantly I add two numbers in front of every string.
It is to store the information of the original string for decompression.

The first number represents the type of the string. [from 0 to 9]
Type 0: It is not the last line and is not compressed.
Type 1: It is not the last line and is compressed with 0 remaining term.
Type 2: It is not the last line and is compressed with 1 remaining term.
Type 3: It is not the last line and is compressed with 2 remaining term.
Type 4: It is not the last line and is compressed with 3 remaining term.
Type 5: It is the last line and is compressed with 0 remaining term.
Type 6: It is the last line and is compressed with 1 remaining term.
Type 7: It is the last line and is compressed with 2 remaining term.
Type 8: It is the last line and is compressed with 3 remaining term.
Type 9: It is the last line and is not compressed.

The second number represents the length of the original string without counting its '\n'.

Every new line character is removed in the compression, as the second numbers, indicating the length of the lines, already separate the lines.

For example.
Having [N][N][N][A]['\n'], I will make it [0][3][78][78][78].
Having [A][T][C][G]['\n'], I will make it [1][4][27].
Having [A][T][C][T][A][A]['\n'], I will make it [3][6][27][0].
Having [N][N], I will make it [9][2][78][78].
and so forth.

**/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUF_SIZE 122


int check_ATCG(unsigned char* str)
{
    int result=1, i, len=strlen(str);

    for(i=0;i<len;i++)
    if(str[i]!='A' && str[i]!='T' && str[i]!='C' && str[i]!='G'&& str[i]!=10)
    {
        result=0;
        break;
    }

    return result;
}

int check_lastline(unsigned char* str)
{
    int result=1, len=strlen(str);
    if(str[len-1]=='\n')
        result=0;

    return result;
}


int convert_ATCG_to_Binary(unsigned char ch)
{
    int BinValue;

    if(ch=='A')
        BinValue=0x0; //00
    else
    if(ch=='T')
        BinValue=0x1; //01
    else
    if(ch=='C')
        BinValue=0x2; //10
    else
    if(ch=='G')
        BinValue=0x3; //11

    return BinValue;
}


unsigned char convert_Binary_to_ATCG(int BinValue)
{
    unsigned char ch;

    if(BinValue==0x0)
        ch='A';
    else
    if(BinValue==0x1)
        ch='T';
    else
    if(BinValue==0x2)
        ch='C';
    else
    if(BinValue==0x3)
        ch='G';

    return ch;
}


int compress(char* original_name, char* compressed_name)
{
    FILE *f_in, *f_out;
    unsigned char buffer[122], compressed[122];
    int lenB, lenC, i, j, NumOfReminder, num;
    unsigned char BinValue, sum;

    f_in=fopen(original_name, "r");
    if(f_in==NULL)
    {
        printf("Input file opening FAILED.");
        return 1;
    }
    f_out=fopen(compressed_name, "wb");
    if(f_out==NULL)
    {
        printf("Output file opening FAILED.");
        return 1;
    }


    while(fgets(buffer, BUF_SIZE, f_in))
    {
        lenB=strlen(buffer);


        if(check_ATCG(buffer)==0)
        {
            if(check_lastline(buffer)==1)
            {
                lenC=lenB+2;
                compressed[0]='9';
                compressed[1]=lenB;
                for(i=0;i<lenB;i++)
                    compressed[i+2]=buffer[i];
            }
            else
            {
                lenC=lenB+1;
                compressed[0]='0';
                compressed[1]=lenB-1;
                for(i=0;i<lenB-1;i++) /** lenB-1 because I don't want to copy the '\n' **/
                    compressed[i+2]=buffer[i];

            }
            compressed[lenC]='\0';
        }
        else
        {
            if(check_lastline(buffer))
            {
                num=lenB/4, NumOfReminder=lenB%4;

                if(NumOfReminder==0)
                    lenC=2+num;
                else
                    lenC=2+num+1;

                compressed[0]=NumOfReminder+5+48;
                compressed[1]=lenB;

                for(j=0;j<num;j++)
                {
                    sum=0;
                    for(i=0;i<4;i++)
                    {
                        BinValue=( convert_ATCG_to_Binary(buffer[i+4*j]) )<<(2*(3-i));
                        sum+=BinValue;
                    }
                    compressed[j+2]=sum;
                }

                if(NumOfReminder!=0)
                {
                    sum=0;
                    for(i=0;i<NumOfReminder;i++)
                    {
                        BinValue=( convert_ATCG_to_Binary(buffer[i+4*num]) )<<(2*(3-i));
                        sum+=BinValue;
                    }
                    compressed[num+2]=sum;
                }

                compressed[lenC]='\0';
            }
            else
            {
                num=(lenB-1)/4, NumOfReminder=(lenB-1)%4; /** lenB-1 because I don't want to consider the '\n' **/

                if(NumOfReminder==0)
                    lenC=2+num;
                else
                    lenC=2+num+1;

                compressed[0]=NumOfReminder+1+48;
                compressed[1]=lenB-1;

                for(j=0;j<num;j++)
                {
                    sum=0;
                    for(i=0;i<4;i++)
                    {
                        BinValue=( convert_ATCG_to_Binary(buffer[i+4*j]) )<<(2*(3-i));
                        sum+=BinValue;
                    }
                    compressed[j+2]=sum;
                }

                if(NumOfReminder!=0)
                {
                    sum=0;
                    for(i=0;i<NumOfReminder;i++)
                    {
                        BinValue=( convert_ATCG_to_Binary(buffer[i+4*num]) )<<(2*(3-i));
                        sum+=BinValue;
                    }
                    compressed[num+2]=sum;
                }

                compressed[lenC]='\0';
            }
        }

        fwrite(compressed, sizeof(unsigned char), lenC, f_out);
    }
    printf("\nDONE!\n");

    fclose(f_in);
    fclose(f_out);
    return 0;
}


int decompress(char* compressed_name, char* decompressed_name)
{
    FILE *f_in, *f_out;
    unsigned char buffer[122], decompressed[122], buffer_type, buffer_len;
    int lenD, i, j, NumOfReminder, num, read;
    unsigned char BinValue;

    f_in=fopen(compressed_name, "rb");
    if(f_in==NULL)
    {
        printf("Input file opening FAILED.");
        return 1;
    }
    f_out=fopen(decompressed_name, "w");
    if(f_out==NULL)
    {
        printf("Output file opening FAILED.");
        return 1;
    }

    while(1)
    {
        buffer_type=fgetc(f_in);
        if(buffer_type==255)
            break;

        buffer_len=fgetc(f_in);

        /** First generalize an integer 'read' to determinate reading how many characters. **/
        if(buffer_type=='0' || buffer_len=='9')
            read=buffer_len;
        else
        {
            NumOfReminder=( (buffer_type-48)-1 )%4;
            num=buffer_len/4;
            if(NumOfReminder==0)
                read=num;
            else
                read=num+1;
        }

        fread(buffer, sizeof(unsigned char), read, f_in);

        if(buffer_type=='0') /** Type 0 **/
        {
            lenD=buffer_len+1;
            decompressed[lenD-1]='\n';

            for(i=0;i<buffer_len;i++)
                decompressed[i]=buffer[i];

            decompressed[lenD]='\0';
        }
        else
        if(buffer_type=='9') /** Type 9 **/
        {
            lenD=buffer_len;
            for(i=0;i<buffer_len;i++)
                decompressed[i]=buffer[i];

            decompressed[lenD]='\0';
        }
        else

        if(buffer_type>='1' && buffer_type<='4') /** Type 1-4 **/
        {
            lenD=buffer_len+1;
            decompressed[lenD-1]='\n';

            for(j=0;j<num;j++)
            {
                for(i=0;i<4;i++)
                {
                    BinValue=( buffer[j]>>( 2*(3-i) ) )&3;
                    decompressed[i+4*j]=convert_Binary_to_ATCG(BinValue);
                }
            }
            if(NumOfReminder!=0)
            {
                for(i=0;i<NumOfReminder;i++)
                {
                    BinValue=( buffer[num]>>( 2*(3-i) ) )&3;
                    decompressed[i+4*num]=convert_Binary_to_ATCG(BinValue);
                }
            }

            decompressed[lenD]='\0';
        }
        else /** Type 5-8 **/
        {
            lenD=buffer_len;

            for(j=0;j<num;j++)
                for(i=0;i<4;i++)
                {
                    BinValue=( buffer[j]>>( 2*(3-i) ) )&3;
                    decompressed[i+4*j]=convert_Binary_to_ATCG(BinValue);
                }

            if(NumOfReminder!=0)
                for(i=0;i<NumOfReminder;i++)
                {
                    BinValue=( buffer[num]>>( 2*(3-i) ) )&3;
                    decompressed[i+4*num]=convert_Binary_to_ATCG(BinValue);
                }

            decompressed[lenD]='\0';
        }

            fwrite(decompressed, sizeof(unsigned char), lenD, f_out);

    }
    printf("\nDONE!\n");

    fclose(f_in);
    fclose(f_out);
    return 0;
}



int main(int argc, char* argv[])
{
    if(argc!=4)
    {
        printf("Commend Arguments Error!\n");
        printf("Usage: project -c input_file compressed_file OR project -d compressed_file decompressed_file\n");
        return 1;
    }
    if(strcmp("-c",argv[1])==0)
        compress(argv[2], argv[3]);
    else
    if(strcmp("-d",argv[1])==0)
        decompress(argv[2], argv[3]);

    return 0;
}
