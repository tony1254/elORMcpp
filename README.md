# elORMcpp
MySQL Eloquent-like ORM para C++ mini proyect

# MySQL Eloquent-like ORM para C++

Este proyecto es una librería en C++ inspirada en el ORM Eloquent de Laravel. Su objetivo es simplificar la interacción con bases de datos MySQL, ofreciendo una interfaz intuitiva para realizar operaciones CRUD (crear, leer, actualizar y eliminar) de forma similar a Eloquent.

## Requisitos

- **Compilador C++:** Compatible con C++11 o superior.
- **MySQL Client Library:** Se requiere tener instalada la librería de cliente MySQL (por ejemplo, libmysqlclient).
  - En Debian/Ubuntu: `sudo apt-get install libmysqlclient-dev`
  - En RedHat/CentOS: `sudo yum install mysql-devel`
- **Sistema Operativo:** Linux, Windows o macOS.

## Instalación

1. **Descargar la librería:**

   Clona o descarga el repositorio:
   ```bash
   git clone https://github.com/tu_usuario/mysql-eloquent-orm-cpp.git

Incluir los archivos en tu proyecto:

Copia los archivos MySQLConexion.h y MySQLModel.h en el directorio de tu proyecto.

Configurar la compilación:

Al compilar tu proyecto, asegúrate de incluir el directorio de los headers y enlazar la biblioteca de MySQL. Por ejemplo, con g++:

bash
Copiar
Editar
g++ -std=c++11 -I/path/to/headers -o mi_proyecto main.cpp -lmysqlclient
Uso
1. Conexión a la Base de Datos
Crea una instancia de MySQLConexion y abre la conexión:

cpp
Copiar
Editar
#include "MySQLConexion.h"

MySQLConexion db("root", "root", "aerolinea");
if (!db.open()) {
    std::cerr << "Error al conectar con la base de datos." << std::endl;
    return 1;
}
2. Configuración del Modelo
Define un modelo para una tabla (por ejemplo, boletos) usando MySQLModel:

cpp
Copiar
Editar
#include "MySQLModel.h"

MySQLModel boleto;
boleto.setConnection(db, "boletos");
boleto.setAttributes({"id", "nombre", "fechaSalida", "fechaLlegada", "asiento", "numeroVuelo"});
3. Operaciones CRUD
Crear un Registro
Asigna valores a los atributos y utiliza el método create():

cpp
Copiar
Editar
boleto.set("nombre", "Juan Perez");
boleto.set("fechaSalida", "2025-03-01 10:00:00");
boleto.set("fechaLlegada", "2025-03-01 14:00:00");
boleto.set("asiento", "12A");
boleto.set("numeroVuelo", "AB123");

if(boleto.create()) {
    std::cout << "Boleto creado correctamente." << std::endl;
}
Leer un Registro
Carga un registro existente mediante el método find(id):

cpp
Copiar
Editar
MySQLModel boletoConsulta;
boletoConsulta.setConnection(db, "boletos");
boletoConsulta.setAttributes({"id", "nombre", "fechaSalida", "fechaLlegada", "asiento", "numeroVuelo"});

if(boletoConsulta.find(1)) {
    std::cout << "Nombre: " << boletoConsulta.get("nombre")
              << ", Asiento: " << boletoConsulta.get("asiento") << std::endl;
} else {
    std::cout << "No se encontró el boleto." << std::endl;
}
Actualizar un Registro
Modifica algún atributo y actualiza el registro con update() (asegúrate de que el campo id esté definido):

cpp
Copiar
Editar
boletoConsulta.set("asiento", "14C");
if(boletoConsulta.update()) {
    std::cout << "Boleto actualizado correctamente." << std::endl;
}
Eliminar un Registro
Elimina un registro usando el método remove():

cpp
Copiar
Editar
if(boletoConsulta.remove()) {
    std::cout << "Boleto eliminado." << std::endl;
}
Obtener Todos los Registros
Recupera todos los registros de la tabla con getAll():

cpp
Copiar
Editar
std::vector< std::map<std::string, std::string> > todos = boleto.getAll();
for(auto &registro : todos) {
    std::cout << "ID: " << registro["id"]
              << ", Nombre: " << registro["nombre"]
              << ", Asiento: " << registro["asiento"] << std::endl;
}
4. Cerrar la Conexión
Una vez finalizadas las operaciones, cierra la conexión:

cpp
Copiar
Editar
db.close();
Funcionalidades
MySQLConexion:

open(): Abre la conexión a la base de datos.
close(): Cierra la conexión.
executeQuery(): Ejecuta consultas SQL (INSERT, UPDATE, DELETE).
executeSelect(): Ejecuta consultas SELECT y retorna los resultados.
getConnection(): Retorna el puntero a la conexión.
MySQLModel:

setConnection(): Asocia el modelo a una conexión y define la tabla.
setAttributes(): Define dinámicamente los nombres de las columnas.
set() / get(): Asigna y obtiene valores de los atributos.
find(id): Busca y carga un registro por el campo id.
create(): Inserta un nuevo registro en la tabla.
update(): Actualiza el registro actual (basado en el campo id).
remove(): Elimina el registro actual (basado en el campo id).
getAll(): Retorna una colección (vector de mapas) de registros de la tabla.
Contribuciones
Si deseas mejorar o ampliar esta librería, puedes enviar pull requests o abrir issues en el repositorio.

Licencia
Este proyecto se distribuye bajo la licencia MIT.
