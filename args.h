#ifndef ARGS_H
#define ARGS_H

enum FORMAT{
	MARKDOWN,
	CSV
};

enum TYPE{
	HEX,
	INT,
    ORIGINAL
};

struct argsdata
{
    char * in_filename;
    char * out_filename;
    enum FORMAT format;
    enum TYPE type;
};

typedef struct argsdata argsdata;

void args_init(argsdata * data);
void args_help(const char *argv0);
void args_usage(const char *argv0);
void args_parse(int argc, char *argv[], argsdata * data);

#endif // ARGS_H
