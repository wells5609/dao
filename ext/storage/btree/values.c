#include "storage/btree/bplus.h"
#include "storage/btree/private/values.h"
#include "storage/btree/private/writer.h"
#include "storage/btree/private/utils.h"

#include "kernel/main.h"


int _dao_storage_btree_value_load(dao_storage_btree_db_t *t,
                   const uint64_t offset,
                   const uint64_t length,
                   dao_storage_btree_value_t *value)
{
    int ret;
    char* buff;
    uint64_t buff_len = length;

    /* read data from disk first */
    ret = _dao_storage_btree_writer_read((_dao_storage_btree_writer_t*) t,
                          kCompressed,
                          offset,
                          &buff_len,
                          (void **) &buff);
    if (ret != DAO_STORAGE_BTREE_OK) return ret;

    value->value = emalloc(buff_len - 16);
    if (value->value == NULL) {
        efree(buff);
        return DAO_STORAGE_BTREE_EALLOC;
    }

    /* first 16 bytes are representing previous value */
    value->_prev_offset = _dao_ntohll(*(uint64_t *) (buff));
    value->_prev_length = _dao_ntohll(*(uint64_t *) (buff + 8));

    /* copy the rest into result buffer */
    memcpy(value->value, buff + 16, buff_len - 16);
    value->length = buff_len - 16;

    efree(buff);

    return DAO_STORAGE_BTREE_OK;
}


int _dao_storage_btree_value_save(dao_storage_btree_db_t *t,
                   const dao_storage_btree_value_t *value,
                   const _dao_storage_btree_kv_t *previous,
                   uint64_t *offset,
                   uint64_t *length)
{
    int ret;
    char* buff;

    buff = emalloc(value->length + 16);
    if (buff == NULL) return DAO_STORAGE_BTREE_EALLOC;

    /* insert offset, length of previous value */
    if (previous != NULL) {
        *(uint64_t *) (buff) = _dao_htonll(previous->offset);
        *(uint64_t *) (buff + 8) = _dao_htonll(previous->length);
    } else {
        *(uint64_t *) (buff) = 0;
        *(uint64_t *) (buff + 8) = 0;
    }

    /* insert current value itself */
    memcpy(buff + 16, value->value, value->length);

    *length = value->length + 16;
    ret = _dao_storage_btree_writer_write((_dao_storage_btree_writer_t *) t,
                           kCompressed,
                           buff,
                           offset,
                           length);
    efree(buff);

    return ret;
}


int _dao_storage_btree_kv_copy(const _dao_storage_btree_kv_t *source, _dao_storage_btree_kv_t *target, int alloc)
{
    /* copy key fields */
    if (alloc) {
        target->value = emalloc(source->length);
        if (target->value == NULL) return DAO_STORAGE_BTREE_EALLOC;

        memcpy(target->value, source->value, source->length);
        target->allocated = 1;
    } else {
        target->value = source->value;
        target->allocated = source->allocated;
    }

    target->length = source->length;

    /* copy rest */
    target->offset = source->offset;
    target->config = source->config;

    return DAO_STORAGE_BTREE_OK;
}
