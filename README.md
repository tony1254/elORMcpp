# elORMcpp EL ARCHIVO del Framework EloquentORM.h y MySQLConexion.h.
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

### 1. Conectar a la Base de Datos

Antes de interactuar con la base de datos, es necesario establecer una conexión:

  ```cpp
      #include "MySQLConexion.h"
      
      MySQLConexion db("usuario", "contraseña", "nombre_base_datos");
      if (!db.open()) {
          cerr << "Error al conectar con la base de datos." << endl;
          return 1;
      }
  ```
A continuación, se presenta la documentación completa en formato Markdown para la librería **EloquentORM.h**, detallando su funcionalidad, métodos disponibles y ejemplos de uso.

---

# EloquentORM.h - Documentación

## Descripción

**EloquentORM.h** es una librería en C++ que proporciona una interfaz sencilla y elegante, inspirada en el ORM Eloquent de Laravel, para interactuar con bases de datos MySQL. Permite realizar operaciones CRUD (Crear, Leer, Actualizar, Eliminar) y ejecutar consultas personalizadas de manera intuitiva.

## Requisitos

- **Compilador C++:** Compatible con C++11 o superior.
- **Librería cliente de MySQL:** [libmysqlclient](https://dev.mysql.com/doc/refman/8.0/en/libmysql-client.html).

## Instalación

1. **Incluir archivos de cabecera:**
   - Asegúrate de tener los archivos `MySQLConexion.h` y `EloquentORM.h` en tu proyecto.

2. **Compilación:**
   - Compila tu proyecto enlazando la librería de MySQL. Por ejemplo, usando g++:

        ```bash
         g++ -std=c++11 -o mi_proyecto main.cpp -lmysqlclient
       ```

---

## Uso

### 1. Conectar a la Base de Datos

Antes de interactuar con la base de datos, es necesario establecer una conexión:

  ```cpp
    #include "MySQLConexion.h"
    
    MySQLConexion db("usuario", "contraseña", "nombre_base_datos");
    if (!db.open()) {
        cerr << "Error al conectar con la base de datos." << endl;
        return 1;
    }
  ```

### 2. Definir el Modelo

Crea una instancia de `EloquentORM` especificando la tabla y sus columnas:

  ```cpp
      #include "EloquentORM.h"
      
      EloquentORM modelo(db, "nombre_tabla", {"columna1", "columna2", "columna3"});
  ```

### 3. Crear un Nuevo Registro

```cpp
modelo.set("columna1", "valor1");
modelo.set("columna2", "valor2");

if (modelo.save()) {
    cout << "Registro creado correctamente." << endl;
}
```

### 4. Buscar un Registro por ID

```cpp
if (modelo.find(1)) {
    cout << "Registro encontrado: " << modelo.get("columna1") << endl;
} else {
    cout << "No se encontró el registro con id 1." << endl;
}
```

### 5. Actualizar un Registro

```cpp
if (modelo.find(1)) {
    modelo.set("columna2", "nuevo_valor");
    if (modelo.save()) {
        cout << "Registro actualizado correctamente." << endl;
    }
}
```

### 6. Eliminar un Registro

```cpp
if (modelo.find(1)) {
    if (modelo.remove()) {
        cout << "Registro eliminado correctamente." << endl;
    }
}
```

### 7. Consultas con Condiciones (`where`)

```cpp
EloquentORM filtro = modelo.where("columna1", "valor1");
vector<map<string, string>> resultados = filtro.getAll();

for (const auto &registro : resultados) {
    for (const auto &campo : registro) {
        cout << campo.first << ": " << campo.second << "  ";
    }
    cout << endl;
}
```

### 8. Consultas Personalizadas (`raw`)

```cpp
EloquentORM consulta = modelo.raw("SELECT columna1, columna2 FROM nombre_tabla WHERE columna1 LIKE '%valor%'");
vector<map<string, string>> resultados = consulta.getAll();

for (const auto &registro : resultados) {
    for (const auto &campo : registro) {
        cout << campo.first << ": " << campo.second << "  ";
    }
    cout << endl;
}
```

## Métodos Disponibles

- `set(const string &field, const string &value)`: Asigna un valor a un campo.
- `get(const string &field)`: Obtiene el valor de un campo.
- `find(int id)`: Busca un registro por su ID.
- `save()`: Guarda el registro actual (inserta o actualiza según corresponda).
- `create()`: Inserta un nuevo registro.
- `update()`: Actualiza el registro actual.
- `remove()`: Elimina el registro actual.
- `where(const string &field, const string &value)`: Aplica una condición para filtrar registros.
- `raw(const string &query)`: Define una consulta SQL personalizada.
- `getAll()`: Obtiene todos los registros que cumplen con la condición o consulta definida.
- `first()`: Obtiene el primer registro que cumple con la condición o consulta definida.

## Notas

- **Seguridad:** Asegúrate de manejar adecuadamente las entradas del usuario para prevenir inyecciones SQL.
- **Flexibilidad:** La librería está diseñada para ser flexible y adaptable a diferentes estructuras de tablas.

## Licencia

Este proyecto se distribuye bajo la licencia MIT.

---

## Contribuciones

Si deseas mejorar o extender esta librería, siéntete libre de enviar pull requests o abrir issues en el repositorio.

---


## Notas Adicionales

- **Personalización:**  
  Puedes utilizar este modelo genérico para cualquier tabla modificando el nombre de la tabla y el vector de campos al instanciar `EloquentORM`.

- **Orden de las Columnas:**  
  Se recomienda que el método `getAll()` utilice los nombres reales de las columnas obtenidos de MySQL para evitar que los datos se asignen incorrectamente en caso de que el orden de las columnas en la base de datos difiera del vector de campos definido.

