#pragma once

#ifdef PL_PLATFORM_WINDOWS

extern polos::application *polos::create_application();

int main(int argc, char **argv)
{
	auto app = polos::create_application();
	app->run();
	delete app;
}

#endif