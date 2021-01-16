# -Wall -Wextra -Winit-self -Wno-missing-field-initializers
COMPILER	= g++
CFLAGS    = -g -MMD -MP -std=c++17
ifeq "$(shell getconf LONG_BIT)" "64"
  LDFLAGS = -pthread 
else
  LDFLAGS = -pthread 
endif
INCDIR		= -I./inc
TARGET		= ./bin/$(shell basename `readlink -f .`)
SRCDIR		= ./src
SOURCES		= $(wildcard $(SRCDIR)/*.cpp)
OBJDIR		= ./load
OBJECTS		= $(addprefix $(OBJDIR)/,$(notdir $(SOURCES:.cpp=.o)))
DEPENDS   	= $(OBJECTS:.o=.d)

$(TARGET): $(OBJECTS)
		$(COMPILER) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	-mkdir -p $(OBJDIR)
	$(COMPILER) $(CFLAGS) $(INCDIR) -o $@ -c $<

clean:
	-rm -f $(OBJECTS) $(DEPENDS) $(TARGET)

test: test
	./test/test.sh

.PHONY: test clean

-include $(DEPENDS)