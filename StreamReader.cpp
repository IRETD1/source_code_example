#include "StreamReader.h"
#include <client/Client.h>
#include <client/ResponseFactory.h>


// Метод чтения потока данных от сервера.
QSharedPointer < Response > StreamReader::readStream( QByteArray &buffer,
                                                      quint32 &recievedRequestSize,
                                                      ResponseReadInfo &info ) {

    QSharedPointer < Response > pResponse( nullptr );
	
	info.setReadStatus( ReadStatus::INPUT_DATA_ARE_INCOMPLETE );
	
    QDataStream stream( buffer );
    stream.setVersion( QDataStream::Qt_5_2 );

    //Если мы не считывали размер запроса
    if ( recievedRequestSize == REQUEST_SIZE_IS_NOT_READ ) {

        //Если клиент прислал размер запроса
        if ( buffer.size() >= sizeof( quint32 ) ) {

            quint32 size = REQUEST_SIZE_IS_NOT_READ;
            stream >> size;
            //запоминаем размер запроса
            recievedRequestSize =  size;
        }
    }
    quint32 sizeRequest = recievedRequestSize;

    //Убеждаемся, что размер запроса был считан и были получены все блоки данных.
    if ( sizeRequest != REQUEST_SIZE_IS_NOT_READ &&
         buffer.size() >= sizeRequest ) {

        quint32 typeRequest;
        stream >> typeRequest;
        ResponseType responseType = static_cast < ResponseType > ( typeRequest );

        ResponseFactory factory;
        pResponse = factory.createResponse( responseType );

        if ( pResponse != NULL ) {

            pResponse.data() -> readStream( stream );
            info.setReadStatus( ReadStatus::INPUT_DATA_ARE_COMPLETE );
        }
        else {

            info.setReadStatus( ReadStatus::INPUT_DATA_ARE_UNEXPECTED );
        }
    }

    QByteArray availableDataFromStream = stream.device() -> readAll();
    buffer = availableDataFromStream;

    return pResponse;

}
