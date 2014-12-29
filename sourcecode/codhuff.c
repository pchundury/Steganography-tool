#include <stdio.h>
/* For routines fgetc,fputc,fwrite and rewind */
#include <memory.h>
/* For routines memset,memcpy */
#include <malloc.h>
/* For routines malloc and free */
#include <stdlib.h>
/* For routines qsort et exit */

/* Error codes returned to the caller */
#define NO_ERROR      0
#define BAD_FILE_NAME 1
#define BAD_ARGUMENT  2
#define BAD_MEM_ALLOC 3

/* Useful constants */
#define FALSE 0
#define TRUE  1

/* Global variables */
FILE *source_file,*dest_file;

typedef struct s_tree { unsigned int byte;
                             /* A byte has to be coded as an unsigned integer to allow a node to have a value over 255 */
                        unsigned long int weight;
                        struct s_tree *left_ptr,
                                      *right_ptr;
                      } t_tree,*p_tree;
#define BYTE_OF_TREE(ptr_tree)  ((*(ptr_tree)).byte)
#define WEIGHT_OF_TREE(ptr_tree)  ((*(ptr_tree)).weight)
#define LEFTPTR_OF_TREE(ptr_tree)  ((*(ptr_tree)).left_ptr)
#define RIGHTPTR_OF_TREE(ptr_tree)  ((*(ptr_tree)).right_ptr)

typedef struct { unsigned char bits[32];
                 unsigned int bits_nb;
               } t_bin_val;
#define BITS_BIN_VAL(bin_val)  ((bin_val).bits)
#define BITS_NB_BIN_VAL(bin_val)  ((bin_val).bits_nb)

                             /* Being that fgetc=EOF only after any access
                                then 'stored_byte_status' is 'TRUE' if a byte has been stored with 'fgetc'
                                or 'FALSE' if there's no valid byte not already read and not handled in 'stored_byte_val' */
int stored_byte_status=FALSE;
int stored_byte_val;

/* Pseudo procedures */
#define beginning_of_data()  { (void)rewind(source_file); stored_byte_status=FALSE; }
#define end_of_data()  (stored_byte_status?FALSE:!(stored_byte_status=((stored_byte_val=fgetc(source_file))!=EOF)))
#define read_byte()  (stored_byte_status?stored_byte_status=FALSE,(unsigned char)stored_byte_val:(unsigned char)fgetc(source_file))
#define write_byte(byte)  ((void)fputc((byte),dest_file))

unsigned char bit_counter_to_write=0;
unsigned int val_to_write=0;

void write_bin_val(bin_val)
/* Returned parameters: None
   Action: Writes in the output stream the value binary-coded into 'bin_val'
   Errors: An input/output error could disturb the running of the program
*/
t_bin_val bin_val;
{ unsigned char bin_pos,
                byte_pos;

  byte_pos=(BITS_NB_BIN_VAL(bin_val)+7) >> 3;
  bin_pos=BITS_NB_BIN_VAL(bin_val) & 7;
  if (bin_pos)
     { byte_pos--;
       val_to_write=(val_to_write<<bin_pos)|BITS_BIN_VAL(bin_val)[byte_pos];
       bit_counter_to_write += bin_pos;
       if (bit_counter_to_write>=8)
          { bit_counter_to_write -= 8;
            write_byte((unsigned char)(val_to_write>>bit_counter_to_write));
            val_to_write &= ((1<<bit_counter_to_write)-1);
          }
     }
  while (byte_pos)
        { byte_pos--;
          val_to_write=(val_to_write<<8)|BITS_BIN_VAL(bin_val)[byte_pos];
          write_byte((unsigned char)(val_to_write>>bit_counter_to_write));
          val_to_write &= ((1<<bit_counter_to_write)-1);
        }
}

void fill_encoding()
/* Returned parameters: None
   Action: Fills the last byte to write in the output stream with zero values
   Errors: An input/output error could disturb the running of the program
*/
{ if (bit_counter_to_write)
     write_byte(val_to_write << (8-bit_counter_to_write));
}

void write_header(codes_table)
/* Returned parameters: None
   Action: Writes the header in the stream of codes
   Errors: An input/output error could disturb the running of the program
*/
t_bin_val codes_table[257];
{ register unsigned int i,j;
  t_bin_val bin_val_to_0,
            bin_val_to_1,
            bin_val;         /* Is used to send in binary mode via write_bin_val */

  *BITS_BIN_VAL(bin_val_to_0)=0;
  BITS_NB_BIN_VAL(bin_val_to_0)=1;
  *BITS_BIN_VAL(bin_val_to_1)=1;
  BITS_NB_BIN_VAL(bin_val_to_1)=1;
  for (i=0,j=0;j<=255;j++)
      if BITS_NB_BIN_VAL(codes_table[j])
         i++;
                             /* From there, i contains the number of bytes of the several non null occurrences to encode */
                              /* First part of the header: Specifies the bytes that appear in the source of encoding */
  if (i<32)
     {                       /* Encoding of the appeared bytes with a block of bytes */
       write_bin_val(bin_val_to_0);
       BITS_NB_BIN_VAL(bin_val)=5;
       *BITS_BIN_VAL(bin_val)=(unsigned char)(i-1);
       write_bin_val(bin_val);
       BITS_NB_BIN_VAL(bin_val)=8;
       for (j=0;j<=255;j++)
           if BITS_NB_BIN_VAL(codes_table[j])
              { *BITS_BIN_VAL(bin_val)=(unsigned char)j;
                write_bin_val(bin_val);
              }
     }
  else {                     /* Encoding of the appeared bytes with a block of bits */
         write_bin_val(bin_val_to_1);
         for (j=0;j<=255;j++)
             if BITS_NB_BIN_VAL(codes_table[j])
                write_bin_val(bin_val_to_1);
             else write_bin_val(bin_val_to_0);
     };
                             /* Second part of the header: Specifies the encoding of the bytes (fictive or not) that appear in the source of encoding */
  for (i=0;i<=256;i++)
      if (j=BITS_NB_BIN_VAL(codes_table[i]))
         { if (j<33)
              { write_bin_val(bin_val_to_0);
                BITS_NB_BIN_VAL(bin_val)=5;
              }
           else { write_bin_val(bin_val_to_1);
                  BITS_NB_BIN_VAL(bin_val)=8;
                }
           *BITS_BIN_VAL(bin_val)=(unsigned char)(j-1);
           write_bin_val(bin_val);
           write_bin_val(codes_table[i]);
         }
}

int weight_tree_comp(tree1,tree2)
/* Returned parameters: Returns a comparison status
   Action: Returns a negative, zero or positive integer depending on the weight
   of 'tree2' is less than, equal to, or greater than the weight of 'tree1'
   Errors: None
*/
p_tree *tree1,*tree2;
{ return (WEIGHT_OF_TREE(*tree2) ^ WEIGHT_OF_TREE(*tree1))?((WEIGHT_OF_TREE(*tree2)<WEIGHT_OF_TREE(*tree1))?-1:1):0;
}

void suppress_tree(tree)
/* Returned parameters: None
   Action: Suppresses the allocated memory for the 'tree'
   Errors: None if the tree has been build with dynamical allocations!
*/
p_tree tree;
{ if (tree!=NULL)
     { suppress_tree(LEFTPTR_OF_TREE(tree));
       suppress_tree(RIGHTPTR_OF_TREE(tree));
       free(tree);
     }
}

p_tree build_tree_encoding()
/* Returned parameters: Returns a tree of encoding
   Action: Generates an Huffman encoding tree based on the data from the stream to compress
   Errors: If no memory is available for the allocations then a 'BAD_MEM_ALLOC' exception is raised
*/
{ register unsigned int i;
  p_tree occurrences_table[257],
         ptr_fictive_tree;

                             /* Sets up the occurrences number of all bytes to 0 */
  for (i=0;i<=256;i++)
      { if ((occurrences_table[i]=(p_tree)malloc(sizeof(t_tree)))==NULL)
           { for (;i;i--)
                 free(occurrences_table[i-1]);
             exit(BAD_MEM_ALLOC);
           }
        BYTE_OF_TREE(occurrences_table[i])=i;
        WEIGHT_OF_TREE(occurrences_table[i])=0;
        LEFTPTR_OF_TREE(occurrences_table[i])=NULL;
        RIGHTPTR_OF_TREE(occurrences_table[i])=NULL;
      }
                             /* Valids the occurrences of 'occurrences_table' with regard to the data to compressr */
  if (!end_of_data())
     { while (!end_of_data())
             { i=read_byte();
               WEIGHT_OF_TREE(occurrences_table[i])++;
             }
       WEIGHT_OF_TREE(occurrences_table[256])=1;
                             /* Sorts the occurrences table depending on the weight of each character */
       (void)qsort(occurrences_table,257,sizeof(p_tree),weight_tree_comp);
       for (i=256;(i!=0)&&(!WEIGHT_OF_TREE(occurrences_table[i]));i--)
           free(occurrences_table[i]);
       i++;
                             /* From there, 'i' gives the number of different bytes with a null occurrence in the stream to compress */
       while (i>0)           /* Looks up (i+1)/2 times the occurrence table to link the nodes in an uniq tree */
             { if ((ptr_fictive_tree=(p_tree)malloc(sizeof(t_tree)))==NULL)
                  { for (i=0;i<=256;i++)
                        suppress_tree(occurrences_table[i]);
                    exit(BAD_MEM_ALLOC);
                  }
               BYTE_OF_TREE(ptr_fictive_tree)=257;
               WEIGHT_OF_TREE(ptr_fictive_tree)=WEIGHT_OF_TREE(occurrences_table[--i]);
               LEFTPTR_OF_TREE(ptr_fictive_tree)=occurrences_table[i];
               if (i)
                  { i--;
                    WEIGHT_OF_TREE(ptr_fictive_tree) += WEIGHT_OF_TREE(occurrences_table[i]);
                    RIGHTPTR_OF_TREE(ptr_fictive_tree)=occurrences_table[i];
                  }
               else RIGHTPTR_OF_TREE(ptr_fictive_tree)=NULL;
               occurrences_table[i]=ptr_fictive_tree;
               (void)qsort((char *)occurrences_table,i+1,sizeof(p_tree),weight_tree_comp);
               if (i)        /* Is there an other node in the occurrence tables? */
                  i++;       /* Yes, then takes care to the fictive node */
             }
     }
  return (*occurrences_table);
}

void encode_codes_table(tree,codes_table,code_val)
/* Returned parameters: The data of 'codes_table' can have been modified
   Action: Stores the encoding tree as a binary encoding table to speed up the access.
   'val_code' gives the encoding for the current node of the tree
   Errors: None
*/
p_tree tree;
t_bin_val codes_table[257],
          *code_val;
{ register unsigned int i;
  t_bin_val tmp_code_val;

  if (BYTE_OF_TREE(tree)==257)
     { if (LEFTPTR_OF_TREE(tree)!=NULL)
                             /* The sub-trees on left begin with an bit set to 1 */
          { tmp_code_val = *code_val;
            for (i=31;i>0;i--)
                BITS_BIN_VAL(*code_val)[i]=(BITS_BIN_VAL(*code_val)[i] << 1)|(BITS_BIN_VAL(*code_val)[i-1] >> 7);
            *BITS_BIN_VAL(*code_val)=(*BITS_BIN_VAL(*code_val) << 1) | 1;
            BITS_NB_BIN_VAL(*code_val)++;
            encode_codes_table(LEFTPTR_OF_TREE(tree),codes_table,code_val);
            *code_val = tmp_code_val;
          };
       if (RIGHTPTR_OF_TREE(tree)!=NULL)
                             /* The sub-trees on right begin with an bit set to 0 */
          { tmp_code_val = *code_val;
            for (i=31;i>0;i--)
                BITS_BIN_VAL(*code_val)[i]=(BITS_BIN_VAL(*code_val)[i] << 1)|(BITS_BIN_VAL(*code_val)[i-1] >> 7);
            *BITS_BIN_VAL(*code_val) <<= 1;
            BITS_NB_BIN_VAL(*code_val)++;
            encode_codes_table(RIGHTPTR_OF_TREE(tree),codes_table,code_val);
            *code_val = tmp_code_val;
          };
     }
  else codes_table[BYTE_OF_TREE(tree)] = *code_val;
}

void create_codes_table(tree,codes_table)
/* Returned parameters: The data in 'codes_table' will be modified
   Action: Stores the encoding tree as a binary encoding table to speed up the access by calling encode_codes_table
   Errors: None
*/
p_tree tree;
t_bin_val codes_table[257];
{ t_bin_val code_val;

  (void)memset((char *)&code_val,0,sizeof(code_val));
  (void)memset((char *)codes_table,0,257*sizeof(*codes_table));
  encode_codes_table(tree,codes_table,&code_val);
}

void huffmanencoding(char * inputfile, char * outputfile)
/* Returned parameters: None
   Action: Compresses with Huffman method all bytes read by the function 'read_byte'
   Errors: An input/output error could disturb the running of the program
*/
{ p_tree tree;
  t_bin_val encoding_table[257];
  unsigned char byte_read;

  if ((source_file=fopen(inputfile,"rb"))==NULL)
	  exit(BAD_FILE_NAME);
  else if ((dest_file=fopen(outputfile,"wb"))==NULL)
      exit(BAD_FILE_NAME);

  if (!end_of_data())
                             /* Generates only whether there are data */
     { tree=build_tree_encoding();
                             /* Creation of the best adapted tree */
       create_codes_table(tree,encoding_table);
       suppress_tree(tree);
                             /* Obtains the binary encoding in an array to speed up the accesses */
       write_header(encoding_table);
                             /* Writes the defintion of the encoding */
       beginning_of_data();  /* Real compression of the data */
       while (!end_of_data())
             { byte_read=read_byte();
               write_bin_val(encoding_table[byte_read]);
             }
       write_bin_val(encoding_table[256]);
                             /* Code of the end of encoding */
       fill_encoding();
                             /* Fills the last byte before closing file, if any */
     }
	
	 printf("Execution of codhuff completed.\n");
	 fclose(source_file);
     fclose(dest_file);
}

