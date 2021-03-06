#include <iostream>
#include <CodeExecutor/Builder.hpp>
#include <CodeExecutor/Process.hpp>
#include <CodeExecutor/CommonCompiler.hpp>
#include <CodeExecutor/CommonLinker.hpp>

static const char* source1Example =
    "#include <iostream>\n"
    "extern \"C\" int func(int a, int b)"
    "{"
    "   std::cout << a + b << std::endl;"
    "}";

static const char* source2Example =
    "extern \"C\" int func2(int a, int b, int c)"
    "{"
    "    return a + b + c;"
    "}";


int main(int argc, char** argv)
{
    CodeExecutor::Builder builder;

    // Setting compiler instance
    builder.setCompiler(
        std::make_shared<CodeExecutor::CommonCompiler>(
            "/usr/bin/gcc"
        )
    );

    // Setting linker instance
    builder.setLinker(
         std::make_shared<CodeExecutor::CommonLinker>(
             "/usr/bin/gcc"
         )
    );

    // Adding build target
    builder.addTarget(

        // Creating source object from actual source
        CodeExecutor::Source::createFromSource(source1Example)
    );

    // Building library
    CodeExecutor::LibraryPtr result;

    try
    {
        result = builder.build();
    }
    catch (std::runtime_error &e)
    {
        // Catching compilation or linkage errors
        std::cerr << "Can't build sources. Error: " << e.what() << std::endl;
        return -1;
    }

    // Resolving function from built library
    auto func = result->resolveFunction<int(int, int)>("func");

    // Checking resolving result
    if (func == nullptr)
    {
        std::cerr << "Can't resolve function: " << result->errorString() << std::endl;
        return 1;
    }

    // Clearing targets
    builder.clearTargets();

    // Adding new target
    builder.addTarget(

        // Catching compilation or linkage errors
        CodeExecutor::Source::createFromSource(source2Example)
    );

    // Building second library
    CodeExecutor::LibraryPtr result2;

    try
    {
        result2 = builder.build();
    }
    catch (std::runtime_error &e)
    {
        // Catching compilation or linkage errors
        std::cerr << "Can't build sources. Error: " << e.what() << std::endl;
        return -1;
    }

    // Resolving second function
    auto func2 = result2->resolveFunction<int(int, int, int)>("func2");

    // Checking resolve result
    if (func2 == nullptr)
    {
        std::cerr << result2->errorString() << std::endl;
        return 1;
    }

    // Running resolved functions
    std::cout << "Executing func(12, 13): ";
    func(12, 13);

    std::cout << "Executing func2(1, 2, 3): " << func2(1, 2, 3) << std::endl;

    return 0;
}