#ifndef CSC1107_7_STRUCTS
#define CSC1107_7_STRUCTS

/*
 * Global variables are declared as static, so are global within the file.
 */

typedef enum hash {
    sha512,
    sha384,
    sha256,
    sha1,
    md5
} hash_t;

typedef struct userspace {
    char original_sentence[BUF_LEN];
    char hashed_sentence[BUF_LEN];
    hash_t hash_type;
    int digest_length;
} userspace_t;

typedef struct hash_result {
    char hashed_sentence[BUF_LEN];
    int match;
} hash_result_t;

#endif
