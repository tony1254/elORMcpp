#include <iostream>
#include "MySQLConexion.h"
#include "EloquentORM.h"

using namespace std;

int main() {
    // 1. Crear la conexión a la base de datos.
    MySQLConexion db("root", "root", "aerolinea");
    if (!db.open()){
        cerr << "Error al conectar con la base de datos." << endl;
        return 1;
    }
    
    // 2. Definir un modelo genérico para la tabla "boletos".
    // Se definen los campos que tendrá la tabla.
    EloquentORM boleto(db, "boletos", { "nombre", "fecha_salida", "fecha_llegada", "asiento", "numero_vuelo"});
    
    // 3. Crear un nuevo registro.
      
    boleto.set("nombre", "Nuevo Registro");
    boleto.set("fecha_salida", "2025-03-01 10:00:00");
    boleto.set("fecha_llegada", "2025-03-01 14:00:00");
    boleto.set("asiento", "12A");
    boleto.set("numero_vuelo", "AB123");
    
    if(boleto.save()){
        cout << "Registro creado correctamente en 'boletos'." << endl;
    }
    
    // 4. Buscar un registro existente por 'id'.
    EloquentORM boletoConsulta(db, "boletos", { "id","nombre", "fecha_salida", "fecha_llegada", "asiento", "numero_vuelo"});
    if(boletoConsulta.find(10)){
        cout << "Registro encontrado: " << boletoConsulta.get("nombre") << endl;
    } else {
        cout << "No se encontró el registro con id 1." << endl;
    }
    
    // 5. Actualizar el registro (suponiendo que se encontró y tiene 'id' asignado).
    boletoConsulta.set("asiento", "UPDATE");
    
    if(boletoConsulta.save()){
        cout << "Registro actualizado correctamente." << endl;
    }
    
    // 6. Obtener y mostrar todos los registros de la tabla "boletos".
    vector< map<string, string> > todos = boleto.getAll();
    cout << "\nListado de registros en 'boletos':" << endl;
    // Iteramos por cada registro (mapa) del vector:
for (const auto &registro : todos) {
    // Para cada registro, iteramos por cada par (campo, valor):
    for (const auto &campo : registro) {
        cout << campo.first << ": " << campo.second << "  ";
    }
    cout << endl; // Salto de línea para separar registros
}
    for(auto &reg : todos) {
        cout << "ID: " << reg["id"] 
             << ", Nombre: " << reg["nombre"]
             << ", Asiento: " << reg["asiento"] << endl;
    }
    // 7. Usar el método where para filtrar registros.
    EloquentORM filtro = boleto.where("nombre", "Juan Perez");
    vector< map<string, string> > resultados = filtro.getAll();

    cout << "\nRegistros filtrados (where nombre = 'Juan Perez'):" << endl;
    for (const auto &registro : resultados) {
        for (const auto &campo : registro) {
            cout << campo.first << ": " << campo.second << "  ";
        }
        cout << endl;
    }

    // 8. Obtener el primer registro que cumpla la condición.
    map<string, string> primerRegistro = filtro.first();
    cout << "\nPrimer registro filtrado:" << endl;
    for (const auto &campo : primerRegistro) {
        cout << campo.first << ": " << campo.second << "  ";
    }
    cout << endl;
    // Uso de consulta raw para obtener todos los registros
    EloquentORM filtroRaw = boleto.raw("SELECT id, nombre FROM boletos WHERE nombre LIKE '%Ana%' AND numero_vuelo = 'VUELO005'");
    vector< map<string, string> > resultadosRaw = filtroRaw.getAll();

    cout << "\nRegistros filtrados (consulta raw):" << endl;
    for (const auto &registro : resultadosRaw) {
        for (const auto &campo : registro) {
            cout << campo.first << ": " << campo.second << "  ";
        }
        cout << endl;
    }

    // Obtener el primer registro de la consulta raw
    map<string, string> primerRegistroRaw = filtroRaw.first();
    cout << "\nPrimer registro filtrado:" << endl;
    for (const auto &campo : primerRegistroRaw) {
        cout << campo.first << ": " << campo.second << "  ";
    }
    cout << endl;
    // 9. Cerrar la conexión.
    db.close();
    return 0;
}
