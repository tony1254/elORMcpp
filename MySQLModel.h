#ifndef MYSQLMODEL_H
#define MYSQLMODEL_H

#include <mysql/mysql.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

#include "MySQLConexion.h"

/**
 * @brief Clase que representa un modelo genérico para interactuar con cualquier tabla de la base de datos.
 *
 * Permite definir dinámicamente los atributos de la tabla y realizar operaciones CRUD (crear, leer, actualizar y eliminar).
 */
class MySQLModel {
private:
    MYSQL *conn;
    string table;
    vector<string> columns;              // Lista de columnas definidas (orden importante)
    map<string, string> attributes;      // Atributos del modelo (par clave-valor)

    /**
     * @brief Función auxiliar para ejecutar una consulta y obtener el resultado.
     * 
     * @param query Consulta SQL a ejecutar.
     * @return MYSQL_RES* Puntero al resultado o nullptr en caso de error.
     */
    MYSQL_RES* execute(const string &query) {
        if(mysql_query(conn, query.c_str())) {
            cerr << "Error en la consulta: " << mysql_error(conn) << endl;
            return nullptr;
        }
        return mysql_store_result(conn);
    }
    
public:
    /**
     * @brief Constructor por defecto.
     */
    MySQLModel() : conn(nullptr) {}
    
    /**
     * @brief Configura la conexión y el nombre de la tabla.
     * 
     * @param db Referencia a la instancia de MySQLConexion.
     * @param tableName Nombre de la tabla en la base de datos.
     */
    void setConnection(MySQLConexion &db, const string &tableName) {
        conn = db.getConnection();
        table = tableName;
    }
    
    /**
     * @brief Define los nombres de las columnas y crea los atributos inicializados en cadena vacía.
     * 
     * @param cols Vector con los nombres de las columnas.
     */
    void setAttributes(const vector<string>& cols) {
        columns = cols;
        for(const auto &col : columns) {
            attributes[col] = "";
        }
    }
    
    /**
     * @brief Asigna un valor a un atributo del modelo.
     * 
     * Si la columna no existe en los atributos definidos, se añade dinámicamente.
     * 
     * @param column Nombre de la columna.
     * @param value Valor a asignar.
     */
    void set(const string &column, const string &value) {
        if(attributes.find(column) != attributes.end()){
            attributes[column] = value;
        } else {
            attributes[column] = value;
            columns.push_back(column);
        }
    }
    
    /**
     * @brief Obtiene el valor de un atributo.
     * 
     * @param column Nombre de la columna.
     * @return string Valor del atributo o cadena vacía si no existe.
     */
    string get(const string &column) {
        if(attributes.find(column) != attributes.end())
            return attributes[column];
        return "";
    }
    
    /**
     * @brief Busca un registro en la tabla por su id.
     * 
     * Se asume que la tabla posee una columna 'id'.
     * 
     * @param id Valor del campo 'id' a buscar.
     * @return true Si se encontró el registro y se actualizaron los atributos.
     * @return false Si no se encontró el registro.
     */
    bool find(int id) {
        string query = "SELECT * FROM " + table + " WHERE id = " + to_string(id) + " LIMIT 1";
        MYSQL_RES *res = execute(query);
        if(res) {
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row) {
                for(size_t i = 0; i < columns.size(); i++) {
                    attributes[columns[i]] = (row[i] ? row[i] : "");
                }
                mysql_free_result(res);
                return true;
            }
            mysql_free_result(res);
        }
        return false;
    }
    
    /**
     * @brief Inserta un nuevo registro en la tabla.
     * 
     * @return true Si la operación fue exitosa.
     * @return false En caso de error.
     */
    bool create() {
        stringstream ss;
        ss << "INSERT INTO " << table << " (";
        // Lista de columnas
        for (size_t i = 0; i < columns.size(); i++) {
            ss << columns[i];
            if(i < columns.size()-1) ss << ", ";
        }
        ss << ") VALUES (";
        // Lista de valores entre comillas simples
        for (size_t i = 0; i < columns.size(); i++) {
            ss << "'" << attributes[columns[i]] << "'";
            if(i < columns.size()-1) ss << ", ";
        }
        ss << ")";
        string query = ss.str();
        if(mysql_query(conn, query.c_str())) {
            cerr << "Error al crear registro: " << mysql_error(conn) << endl;
            return false;
        }
        return true;
    }
    
    /**
     * @brief Actualiza el registro actual en la tabla.
     * 
     * Se requiere que el atributo 'id' esté definido.
     * 
     * @return true Si la actualización fue exitosa.
     * @return false En caso de error o si 'id' no está definido.
     */
    bool update() {
        if(attributes.find("id") == attributes.end() || attributes["id"].empty()) {
            cerr << "Error al actualizar: 'id' no está definido." << endl;
            return false;
        }
        stringstream ss;
        ss << "UPDATE " << table << " SET ";
        bool first = true;
        for (const auto &col : columns) {
            if(col == "id") continue; // Se omite el campo 'id'
            if(!first) ss << ", ";
            ss << col << " = '" << attributes[col] << "'";
            first = false;
        }
        ss << " WHERE id = " << attributes["id"];
        string query = ss.str();
        if(mysql_query(conn, query.c_str())) {
            cerr << "Error al actualizar: " << mysql_error(conn) << endl;
            return false;
        }
        return true;
    }
    
    /**
     * @brief Elimina el registro actual de la tabla.
     * 
     * Se requiere que el atributo 'id' esté definido.
     * 
     * @return true Si la eliminación fue exitosa.
     * @return false En caso de error o si 'id' no está definido.
     */
    bool remove() {
        if(attributes.find("id") == attributes.end() || attributes["id"].empty()) {
            cerr << "Error al eliminar: 'id' no está definido." << endl;
            return false;
        }
        string query = "DELETE FROM " + table + " WHERE id = " + attributes["id"];
        if(mysql_query(conn, query.c_str())) {
            cerr << "Error al eliminar: " << mysql_error(conn) << endl;
            return false;
        }
        return true;
    }
    
    /**
     * @brief Obtiene todos los registros de la tabla.
     * 
     * @return vector< map<string, string> > Vector de mapas, donde cada mapa representa un registro con pares campo-valor.
     */
    vector< map<string, string> > getAll() {
        vector< map<string, string> > rows;
        string query = "SELECT * FROM " + table;
        MYSQL_RES *res = execute(query);
        if(res) {
            MYSQL_ROW row;
            unsigned int num_fields = mysql_num_fields(res);
            while((row = mysql_fetch_row(res))) {
                map<string, string> record;
                for (size_t i = 0; i < columns.size() && i < num_fields; i++) {
                    record[columns[i]] = (row[i] ? row[i] : "");
                }
                rows.push_back(record);
            }
            mysql_free_result(res);
        }
        return rows;
    }
};

#endif // MYSQLMODEL_H
