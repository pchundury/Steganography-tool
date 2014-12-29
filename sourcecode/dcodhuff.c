#include <stdio.h>
/* For routines printf,fgetc and fputc */
#include <memory.h>
/* For routine memset */
#include <malloc.h>
/* For routines malloc and free */
#include <stdlib.h>
/* For routine exit */

/* Error codes returned to the caller */
#define NO_ERROR      0
#define BAD_FILE_NAME 1
#define BAD_ARGUMENT  2
#define BAD_MEM_ALLOC 3

/*  Useful constants */
#define FALSE 0
#define TRUE  1

typedef struct s_tree { unsigned int byte;
                             /* A byte has to be coded as an unsigned integer to allow a node to have a value over 255 */
                         struct s_tree *left_ptr,
                                       *right_ptr;
                       } t_tree,*p_tree;
#define TREE_BYTE(ptr_tree)  ((*(ptr_tree)).byte)
#define LEFTPTR_OF_TREE(ptr_tree)  ((*(ptr_tree)).left_ptr)
#define RIGHTPTR_OF_TREE(ptr_tree)  ((*(ptr_tree)).right_ptr)

typedef struct { unsigned char bits[32];
                 unsigned int bits_nb;
                 unsigned char presence;
               } t_bin_val;
#define BITS_BIN_VAL(x)  ((x).bits)
#define NB_BITS_BIN_VAL(x)  ((x).bits_nb)
#define PRESENCE_BIN_VAL(x)  ((x).presence)

/* Global variables */
FILE *source_file,*dest_file;

                             /* Being that fgetc=EOF only after an access
                                then 'byte_stored_status' is 'TRUE' if a byte has been stored by 'fgetc'
                                or 'FALSE' if there's no valid byte not handled in 'val_byte_stored' */
int byte_stored_status=FALSE;
int byte_stored_val;

/* Pseudo procedures */
#define end_of_data()  (byte_stored_status?FALSE:!(byte_stored_status=((byte_stored_val=fgetc(source_file))!=EOF)))
#define read_byte()  (byte_stored_status?byte_stored_status=FALSE,(unsigned char)byte_stored_val:(unsigned char)fgetc(source_file))
#define write_byte(byte)  ((void)fputc((byte),dest_file))

unsigned char bit_counter_to_read=0;
unsigned int val_to_read=0;

unsigned char read_code_1_bit()
/* Returned parameters: Returns an unsigned integer with the 0-bit (on the right of the integer) valid
   Action: Reads the next bit in the stream of data to compress
   Errors: An input/output error could disturb the running of the program
   The source must have enough bits to read
*/
{ unsigned int result;

  if (bit_counter_to_read)
     { bit_counter_to_read--;
       result=(val_to_read >> bit_counter_to_read) & 1;
     }
  else { val_to_read=read_byte();
         bit_counter_to_read=7;
         result=(val_to_read >> 7) & 1;
       }
  val_to_read &= 127;
  return result;
}

unsigned int read_code_n_bits(n)
/* Returned parameters: Returns an unsigned integer with the n-bits (on the right of the integer) valid
   Action: Reads the next n bits in the stream of data to compress
   Errors: An input/output error could disturb the running of the program
   The source must have enough bits to read
*/
unsigned char n;
{ unsigned int result;

  while ((bit_counter_to_read<n)&&(!end_of_data()))
        { val_to_read=(val_to_read << 8)+read_byte();
          bit_counter_to_read += 8;
        }
  bit_counter_to_read -= n;
  result=val_to_read >> bit_counter_to_read;
  val_to_read &= ((1<<bit_counter_to_read)-1);
  return result;
}

void read_header(codes_table)
/* Returned parameters: The contain of 'codes_table' is modified
   Action: Rebuilds the binary encoding array by using the header
   Errors: An input/output error could disturb the running of the program
*/
t_bin_val codes_table[257];
{ register unsigned int i,j;
  char byte_nb;

  (void)memset((char *)codes_table,0,257*sizeof(*codes_table));
                             /* == Decoding of the first part of the header === */
  if (read_code_1_bit())
                             /* First bit=0 => Present bytes coded on n*8 bits
                                         =1 => Present bytes coded on 256 bits */
     for (i=0;i<=255;i++)
         PRESENCE_BIN_VAL(codes_table[i])=read_code_1_bit();
  else { i=read_code_n_bits(5)+1;
         while (i)
               { PRESENCE_BIN_VAL(codes_table[read_code_n_bits(8)])=1;
                 i--;
               }
       }
  PRESENCE_BIN_VAL(codes_table[256])=1;
                             /* Presence of a fictive 256-byte is enforced! */

                             /* == Decoding the second part of the header == */
  for (i=0;i<=256;i++)
      if PRESENCE_BIN_VAL(codes_table[i])
         { if (read_code_1_bit())
                             /* First bit=0 => 5 bits of binary length-1 followed by a binary word
                                         =1 => 8 bits of binary length-1 followed by a binary word */
              j=read_code_n_bits(8)+1;
           else j=read_code_n_bits(5)+1;
           NB_BITS_BIN_VAL(codes_table[i])=j;
                             /* Reading of a binary word */
           byte_nb=(j+7) >> 3;
           if (j & 7)
              {              /* Reads the bits that takes less than one byte */
                byte_nb--;
                BITS_BIN_VAL(codes_table[i])[byte_nb]=(unsigned char)read_code_n_bits(j & 7);
              }
           while (byte_nb)
                 {           /* Reads the bits that takes one byte, at least */
                   byte_nb--;
                   BITS_BIN_VAL(codes_table[i])[byte_nb]=(unsigned char)read_code_n_bits(8);
                 }
         }
}

void suppress_tree(tree)
/* Returned parameters: None
   Action: Suppresses the allocated memory for the tree
   Errors: None if the tree has been build with dynamical allocations!
*/
p_tree tree;
{ if (tree!=NULL)
     { suppress_tree(LEFTPTR_OF_TREE(tree));
       suppress_tree(RIGHTPTR_OF_TREE(tree));
       free(tree);
     }
}

p_tree tree_encoding(codes_table)
/* Returned parameters: A binary tree is returned
   Action: Returns the decoding binary tree built from 'codes_table'
   Errors: None
*/
t_bin_val codes_table[257];
{ register unsigned int i;
  unsigned int j;
  p_tree ptr_tree,current_node;

  if ((ptr_tree=(p_tree)malloc(sizeof(t_tree)))==NULL)
     exit(BAD_MEM_ALLOC);
  TREE_BYTE(ptr_tree)=257;
  LEFTPTR_OF_TREE(ptr_tree)=NULL;
  RIGHTPTR_OF_TREE(ptr_tree)=NULL;
  for (i=0;i<=256;i++)
      { for (current_node=ptr_tree,j=NB_BITS_BIN_VAL(codes_table[i]);j;j--)
          { if (BITS_BIN_VAL(codes_table[i])[(j-1) >> 3] & (1 << ((j-1) & 7)))
               if (LEFTPTR_OF_TREE(current_node)==NULL)
                  { if ((LEFTPTR_OF_TREE(current_node)=(p_tree)malloc(sizeof(t_tree)))==NULL)
                       { suppress_tree(ptr_tree);
                         exit(BAD_MEM_ALLOC);
                       }
                    current_node=LEFTPTR_OF_TREE(current_node);
                    LEFTPTR_OF_TREE(current_node)=NULL;
                    RIGHTPTR_OF_TREE(current_node)=NULL;
                  }
               else current_node=LEFTPTR_OF_TREE(current_node);
            else if (RIGHTPTR_OF_TREE(current_node)==NULL)
                    { if ((RIGHTPTR_OF_TREE(current_node)=(p_tree)malloc(sizeof(t_tree)))==NULL)
                       { suppress_tree(ptr_tree);
                         exit(BAD_MEM_ALLOC);
                       }
                      current_node=RIGHTPTR_OF_TREE(current_node);
                      LEFTPTR_OF_TREE(current_node)=NULL;
                      RIGHTPTR_OF_TREE(current_node)=NULL;
                    }
                 else current_node=RIGHTPTR_OF_TREE(current_node);
            if (j==1)
               TREE_BYTE(current_node)=i;
            else TREE_BYTE(current_node)=257;
          }
      };
  return (ptr_tree);
}

void huffmandecoding(char * inputfile, char * outputfile)
/* Returned parameters: None
   Action: Decompresses with Huffman method all bytes read by the function 'read_code_1_bit' and 'read_code_n_bits'
   Errors: An input/output error could disturb the running of the program
*/
{ t_bin_val encoding_table[257];
  p_tree ptr_tree,current_node;

  if ((source_file=fopen(inputfile,"rb"))==NULL)
	  exit(BAD_FILE_NAME);
  else if ((dest_file=fopen(outputfile,"wb"))==NULL)
      exit(BAD_FILE_NAME);

  if (!end_of_data())    /* Are there data to compress? */
     { read_header(encoding_table);
       ptr_tree=tree_encoding(encoding_table);
       do { current_node=ptr_tree;
            while (TREE_BYTE(current_node)==257)
                  if (read_code_1_bit())
                             /* Bit=1 => Got to left in the node of the tree
                                   =0 => Got to right in the node of the tree */
                     current_node=LEFTPTR_OF_TREE(current_node);
                  else current_node=RIGHTPTR_OF_TREE(current_node);
            if (TREE_BYTE(current_node)<=255)
               write_byte(TREE_BYTE(current_node));
          }
       while (TREE_BYTE(current_node)!=256);
       suppress_tree(ptr_tree);
     }
  	 
  printf("Execution of decodhuff completed.\n");
  fclose(source_file);
  fclose(dest_file);
}


