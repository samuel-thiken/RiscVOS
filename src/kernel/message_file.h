#ifndef __MESSAGE_FILE_H__
#define __MESSAGE_FILE_H__

#include "buf.h"
#include "liste.h"
#include "process.h"
#include "stdbool.h"

#define NBQUEUE 30 // NB de queue total pouvant être créé par pcreate

typedef struct msg_t {
  STRUCT_BUFFER(int) msgs; // "liste des messages" de taille COUNT
   list_t *process_waiting; // liste des process en attente de message
} msg_t;

int getnewfid();
int valid_fid(int fid);

// doc des fonctions : https://gitlab.ensimag.fr/pcserv/documentation-risc-v/-/wikis/specification#pcount-obtention-dinformation-sur-une-file-de-messages-pcount___obtention_dinformation_sur_une_file_de_messages
int pcount(int fid, int *count);
int pcreate(int count);
int pdelete(int fid);
int preceive(int fid, int *message);
int preset(int fid);
int psend(int fid, int message);


#endif
