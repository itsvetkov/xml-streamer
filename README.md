XML stream writer
=================

XML stream data writer class for easy structured logging.

Usage example:
```cpp
xmls::xmlstreamer xml(std::wcout);

xml
	<< xmls::tag(L"root")

		<< xmls::tag(L"data")
			<< xmls::attr(L"one") << L"foo"
			<< xmls::attr(L"two") << 42
				<< "Text data"
		<< xmls::close()

		<< xmls::tag(L"data")
			<< xmls::attr(L"one") << 3.1415
			<< xmls::attr(L"two") << L"bar"
		<< xmls::close()

	<< xmls::close();
```

Produced output:
```xml
<?xml version="1.0" encoding="UTF-16"?>
<root>
	<data one="foo" two="42">Text data</data>
	<data one="3.1415" two="bar" />
</root>
```

© 2012, Ilya Tsetkov