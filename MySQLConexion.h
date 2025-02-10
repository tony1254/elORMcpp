#ifndef MYSQLCONEXION_H
#define MYSQLCONEXION_H

#include <mysql/mysql.h>
#include <string>
#include <iostream>

using namespace std;

/**
 * @brief Clase que maneja la conexión a una base de datos MySQL.
 *
 * Proporciona métodos para abrir y cerrar la conexión, ejecutar consultas y obtener resultados.
 */
class MySQLConexion {
private:
    MYSQL *conn;
    string host;
    string user;
    string password;
    string database;
    unsigned int port;
public:
    /**
     * @brief Constructor.
     * 
     * @param user Nombre de usuario.
     * @param password Contraseña del usuario.
     * @param database Nombre de la base de datos.
     * @param host Host de la base de datos (por defecto: "localhost").
     * @param port Puerto de conexión (por defecto: 3306).
     */
    MySQLConexion(const string &user, const string &password, const string &database, const string &host = "localhost", unsigned int port = 3306)
        : user(user), password(password), database(database), host(host), port(port) {
        conn = mysql_init(nullptr);
    }
    
    /**
     * @brief Abre la conexión a la base de datos.
     * 
     * @return true Si se estableció la conexión correctamente.
     * @return false En caso de error.
     */
    bool open() {
        if (!mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(), database.c_str(), port, NULL, 0)) {
            cerr << "Error de conexión: " << mysql_error(conn) << endl;
            return false;
        }
        return true;
    }
    
    /**
     * @brief Cierra la conexión a la base de datos.
     */
    void close() {
        if(conn) {
            mysql_close(conn);
            conn = nullptr;
        }
    }
    
    /**
     * @brief Ejecuta una consulta SQL (INSERT, UPDATE, DELETE, etc.) que no retorna resultados.
     * 
     * @param query La consulta SQL a ejecutar.
     * @return true Si la consulta se ejecutó correctamente.
     * @return false En caso de error.
     */
    bool executeQuery(const string &query) {
        if(mysql_query(conn, query.c_str())) {
            cerr << "Error en la consulta: " << mysql_error(conn) << endl;
            return false;
        }
        return true;
    }
    
    /**
     * @brief Ejecuta una consulta SELECT y retorna el resultado.
     * 
     * @param query La consulta SELECT a ejecutar.
     * @return MYSQL_RES* Puntero al resultado de la consulta o nullptr en caso de error.
     */
    MYSQL_RES* executeSelect(const string &query) {
        if(mysql_query(conn, query.c_str())) {
            cerr << "Error en la consulta: " << mysql_error(conn) << endl;
            return nullptr;
        }
        return mysql_store_result(conn);
    }
    
    /**
     * @brief Retorna el puntero a la conexión.
     * 
     * @return MYSQL* Puntero a la conexión MySQL.
     */
    MYSQL* getConnection() {
        return conn;
    }
    
    /**
     * @brief Destructor.
     */
    ~MySQLConexion() {
        close();
    }
};

#endif // MYSQLCONEXION_H
