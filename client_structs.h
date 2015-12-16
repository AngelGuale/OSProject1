// estructura para argumentos para un nuevo subscriber thread
typedef struct
{
    void *context;
    char *filter;
    char *id;
} subscriber_struct;

subscriber_struct *subscriber_struct_create(void *c, char *f, char *id){
    subscriber_struct *s_struct; 
    s_struct = malloc(sizeof *s_struct);

    s_struct->context = c;
    s_struct->filter = f;
    s_struct->id = id;
}
