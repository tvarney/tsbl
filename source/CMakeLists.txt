
set(SOURCE_TSBL
  ./source/interpreter.cpp
  ./source/lexer.cpp
  ./source/token.cpp
  ./source/utf8.cpp
)

set(SOURCE_REPL
  ./source/repl.cpp
)

set(SOURCE_LIB
  ${SOURCE_TSBL}
)

set(SOURCE_TSBL ${SOURCE_TSBL} PARENT_SCOPE)
set(SOURCE_REPL ${SOURCE_REPL} PARENT_SCOPE)
set(SOURCE_LIB ${SOURCE_LIB} PARENT_SCOPE)
