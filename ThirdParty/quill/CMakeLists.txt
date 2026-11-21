cpmaddpackage(
    NAME quill
    VERSION 10.2.0
    URL https://codeload.github.com/odygrd/quill/tar.gz/refs/tags/v10.2.0
    URL_HASH SHA256=d0756ce6716e01c48e52588cdec12b7c77854204bf96f27fb683fd6c11a6be13
    SYSTEM YES
    EXCLUDE_FROM_ALL YES
    OPTIONS "QUILL_NO_EXCEPTIONS ON" "QUILL_DISABLE_NON_PREFIXED_MACROS ON"
)

install(FILES "${quill_SOURCE_DIR}/LICENSE"
    DESTINATION "${POLOS_INSTALL_DIR}"
    RENAME LICENSE-Quill.txt
    COMPONENT Runtime
)
