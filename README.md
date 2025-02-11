# elORMcpp
MySQL Eloquent-like ORM para C++ mini proyect

# MySQL Eloquent-like ORM para C++

Este proyecto es una librería en C++ inspirada en el ORM Eloquent de Laravel. Su objetivo es simplificar la interacción con bases de datos MySQL, ofreciendo una interfaz intuitiva para realizar operaciones CRUD (crear, leer, actualizar y eliminar) de forma similar a Eloquent.
A continuación se muestra un ejemplo completo de documentación en formato Markdown (README.md) para el proyecto, que incluye descripción, requisitos, instrucciones de instalación, uso y ejemplos de código. Puedes copiar y pegar este contenido en el archivo README.md de tu repositorio en GitHub.

---

```markdown
# Eloquent-like ORM para C++ (MySQL)

Esta librería ofrece una interfaz simple y elegante, inspirada en el ORM Eloquent de Laravel, para interactuar con bases de datos MySQL en C++.  
La solución se basa en dos archivos de cabecera:

- **MySQLConexion.h**: Gestiona la conexión a la base de datos.
- **EloquentORM.h**: Implementa un modelo genérico para realizar operaciones CRUD (crear, leer, actualizar, eliminar) sobre cualquier tabla, sin depender de una clase específica para cada una.

> **Nota:** La librería utiliza la API de MySQL (libmysqlclient). Asegúrate de tenerla instalada en tu sistema.

---

## Requisitos

- **Compilador C++:** Compatible con C++11 o superior.
- **Librería cliente de MySQL:** Por ejemplo, [libmysqlclient](https://dev.mysql.com/doc/refman/8.0/en/libmysql-client.html).  
  - En Debian/Ubuntu:  
    ```bash
    sudo apt-get install libmysqlclient-dev
    ```
  - En RedHat/CentOS:  
    ```bash
    sudo yum install mysql-devel
    ```

---

## Instalación

1. **Descarga o clona el repositorio:**
   ```bash
   git clone https://github.com/tu_usuario/eloquent-orm-cpp.git
   ```

2. **Incluye los archivos en tu proyecto:**
   - Copia `MySQLConexion.h` y `EloquentORM.h` en el directorio de tu proyecto.

3. **Compila tu proyecto enlazando la librería MySQL.**  
   Ejemplo usando g++:
   ```bash
   g++ -std=c++11 -I/path/to/headers -o mi_proyecto main.cpp -lmysqlclient
   ```

---

## Uso

Puedes utilizar la librería de forma genérica para cualquier tabla. En el siguiente ejemplo se muestra cómo interactuar con la tabla `boletos`.

### Ejemplo en `main.cpp`

```cpp
#include <iostream>
#include "MySQLConexion.h"
#include "EloquentORM.h"

using namespace std;

int main() {
    // 1. Crear y abrir la conexión a la base de datos.
    MySQLConexion db("root", "root", "aerolinea");
    if (!db.open()){
        cerr << "Error al conectar con la base de datos." << endl;
        return 1;
    }
    
    // 2. Definir el modelo genérico para la tabla "boletos" con los campos definidos.
    //    (Los campos pueden ser de cualquier tabla, simplemente cambia el nombre de la tabla y los campos según sea necesario)
    EloquentORM boleto(db, "boletos", {"id", "nombre", "fechaSalida", "fechaLlegada", "asiento", "numeroVuelo"});
    
    // 3. Crear un nuevo registro.
    boleto.set("nombre", "Juan Perez");
    boleto.set("fechaSalida", "2025-03-01 10:00:00");
    boleto.set("fechaLlegada", "2025-03-01 14:00:00");
    boleto.set("asiento", "12A");
    boleto.set("numeroVuelo", "AB123");
    
    if(boleto.save()){
        cout << "Registro creado correctamente en 'boletos'." << endl;
    }
    
    // 4. Buscar un registro existente por 'id'.
    EloquentORM boletoConsulta(db, "boletos", {"id", "nombre", "fechaSalida", "fechaLlegada", "asiento", "numeroVuelo"});
    if(boletoConsulta.find(1)){
        cout << "Registro encontrado: " << boletoConsulta.get("nombre") << endl;
    } else {
        cout << "No se encontró el registro con id 1." << endl;
    }
    
    // 5. Actualizar el registro (suponiendo que 'id' ya está definido en boletoConsulta).
    boletoConsulta.set("asiento", "14C");
    if(boletoConsulta.save()){
        cout << "Registro actualizado correctamente." << endl;
    }
    
    // 6. Obtener y mostrar todos los registros de la tabla "boletos".
    vector< map<string, string> > todos = boleto.getAll();
    cout << "\nListado de registros en 'boletos':" << endl;
    for (const auto &registro : todos) {
        for (const auto &campo : registro) {
            cout << campo.first << ": " << campo.second << "  ";
        }
        cout << endl;
    }
    
    // 7. Cerrar la conexión.
    db.close();
    return 0;
}
```

### Explicación de las Funcionalidades

- **MySQLConexion.h**
  - `open()`: Abre la conexión a la base de datos.
  - `close()`: Cierra la conexión.
  - `executeQuery(query)`: Ejecuta consultas que no retornan datos (INSERT, UPDATE, DELETE).
  - `executeSelect(query)`: Ejecuta consultas SELECT y retorna el resultado.
  - `getConnection()`: Retorna el puntero a la conexión MySQL.

- **EloquentORM.h**
  - **Constructor:** Recibe la conexión, el nombre de la tabla y un vector de campos.
  - `set(field, value)`: Asigna un valor a un campo.
  - `get(field)`: Retorna el valor de un campo.
  - `find(id)`: Busca un registro por el campo `id` y carga sus atributos.
  - `save()`: Guarda el registro. Si el campo `id` está vacío, crea uno nuevo; de lo contrario, actualiza el existente.
  - `create()`: Inserta un nuevo registro.
  - `update()`: Actualiza el registro actual.
  - `remove()`: Elimina el registro actual.
  - `getAll()`: Retorna todos los registros de la tabla como un vector de mapas.  
    **Importante:** El método `getAll()` ha sido modificado para obtener dinámicamente los nombres reales de las columnas utilizando `mysql_fetch_fields()`, asegurando que los datos se asignen correctamente sin depender del orden del vector.

---

## Contribuciones

Si deseas mejorar o extender esta librería, siéntete libre de enviar pull requests o abrir issues en el repositorio.

---

## Licencia

Este proyecto se distribuye bajo la licencia MIT.

---

## Notas Adicionales

- **Personalización:**  
  Puedes utilizar este modelo genérico para cualquier tabla modificando el nombre de la tabla y el vector de campos al instanciar `EloquentORM`.

- **Orden de las Columnas:**  
  Se recomienda que el método `getAll()` utilice los nombres reales de las columnas obtenidos de MySQL para evitar que los datos se asignen incorrectamente en caso de que el orden de las columnas en la base de datos difiera del vector de campos definido.

