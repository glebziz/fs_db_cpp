# FS DB cpp

FS DB cpp is a cpp client for FS DB. [Source code](https://github.com/glebziz/fs_db) of the FS DB Server. 

## Usage

### Example

See more examples [here](https://github.com/glebziz/fs_db_cpp/tree/master/example/).

```c++
#include <iostream>
#include <utility>

#include <fs_db/connection.h>
#include <fs_db/transaction.h>
#include <fs_db/write_file.h>
#include <fs_db/read_file.h>
#include <fs_db/exception.h>

int main() {
    const fs_db::Connection conn("localhost:8888");

    const std::string key = "someKey";
    const std::string data = "some content";
    const std::string data2 = "some content2";

    try {
        fs_db::WriteFile wf = conn.Set(key);
        wf.Write(data);
        wf.Write(data2);
        wf.Close();

        auto rf = conn.GetFile(key);

        size_t n;
        char d[10];
        std::string content;
        while ((n = rf.Read(d, sizeof(d))) > 0) {
            content.append(d, n);
        }

        std::cout << content.data() << std::endl;

        conn.Delete(key);

        {
            auto tx = conn.Begin(fs_db::ISO_LEVEL_SERIALIZABLE);
            tx.Set(key, data2);

            const fs_db::Transaction tx2(std::move(tx));

            content = tx2.Get(key);
            std::cout << content << std::endl;
        }

        content.clear();
        rf = conn.GetFile(key);
        while ((n = rf.Read(d, sizeof(d))) > 0) {
            content.append(d, n);
        }

        std::cout << content.data() << std::endl;
    } catch (fs_db::Exception &ex) {
        std::cout << ex.what() << std::endl;
    }

    return 0;
}
```

## License

[MIT](https://choosealicense.com/licenses/mit/)