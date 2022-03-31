#
# Общие правила для сборки приложений в BARDY
# Файл app.mk должен быть включен в Makefile собираемого
# приложения. В Makefile приложения необходимо определить
# следующие переменные:
#
# OBJ_FILES - список объектных файлов для линковки приложения
# TARGET_NAME := deasy - имя собираемого приложения
# GIPCYLIB - директория с библиотекой libgipcy.so
# BARDYLIB - директория с библиотекой libbrd.so
#

LIBS += -lbrd -lgipcy  -lstdc++ -ldl -lpthread -lc -lrt -lm
LDOPTIONS := -Wl,-rpath,$(GIPCYLIB) -Wl,-rpath,$(BARDYLIB) -L"$(GIPCYLIB)" -L"$(BARDYLIB)" $(LIBS)

$(TARGET_NAME): $(OBJ_FILES)
	$(LD) -o $(TARGET_NAME) $(notdir $(OBJ_FILES)) $(LDOPTIONS)
