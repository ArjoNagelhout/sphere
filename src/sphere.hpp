#include <string>
#include <vector>

#ifndef EDITOR_HPP
#define EDITOR_HPP

namespace editor
{
	class Editor
	{
	public:
        void initialize();
        void cleanUp();

		void printStrings();

	private:
		std::vector<std::string> strings;
	};
}

#endif //EDITOR_HPP