<?php
/**
 * Parses My Feed
 */
class CustomParser extends FeedsParser {

  private function cleanupHtml($string){
    $my_allowed_tags = array('a', 'em', 'strong', 'cite', 'blockquote', 'code', 'ul', 'ol', 'li', 'dl', 'dt', 'dd', 'img');
    return filter_xss($string, $my_allowed_tags);
  }

  private function cleanupUrl($url){
    return check_url($url);
  }

  public function parse(FeedsSource $source, FeedsFetcherResult $fetcher_result) {

    // Get the file's content.
    $string = $fetcher_result->getRaw();
    $result = new FeedsParserResult();
    $items = array();

    //parsing... The Verge [http://www.theverge.com/rss/frontpage]
    //TODO: extract img tags from content element for image field
    try {
      $feed = new SimpleXMLElement($string);

      foreach ($feed as $entry) {			
        $name = $entry->getName();

        if($name == 'entry'){
          $guid = '';
          $title = '';
          $author = '';
          $description = '';
          $images = array();
          $video = '';
          $link = '';
          $published = '';

          foreach ($entry as $element){
            $elementName = $element->getName();

            switch ($elementName) {
              case "id":
                $guid = sprintf("%s",$element);
                break;
              case "title":
                $title = sprintf("%s",$element);
                break;
              case "content":
                $description = sprintf("%s",$element);
                break;
              case "published":
                $published = sprintf("%s",$element);
                break;
              case "link":
                $is_video = false;
                foreach($element->attributes() as $key => $value){
                  if($key == 'type' && $value == 'text/html') {
                    $is_video = false;
                  }

                  if($key == 'type' && $value == 'video/mp4') {
                    $is_video = true;
                  }

                  if($key == 'href'){
                    if($is_video){
                      $video = $value;
                    }else{
                      $link = $value;
                    }
                  }
                }
                break;
              case "author":
                $author = sprintf("%s",$element->name);
                break;
            }
          }

          $items[] = array(
            'guid' => self::cleanupUrl($guid),
            'published' => $published,
            'title' => self::cleanupHtml($title),
            'description' => self::cleanupHtml($description),
            'link' => self::cleanupUrl($link),
            'video' => self::cleanupUrl($video),
            'author' => self::cleanupHtml($author),
          );
        }
      }

    } catch (Exception $e) {
      print var_export($source);
      print $e;
    }

    $result->title = 'Custom XML parser';
    $result->items = $items;

    return $result;

  }

  /**
   * Add the extra mapping sources provided by this parser.
   */
  public function getMappingSources() {
    return parent::getMappingSources() + array(
      'guid' => array (
        'name' => t('guid'),
        'description' => t('guid'),
      ),
      'published' => array (
        'name' => t('published'),
        'description' => t('published date'),
      ),
      'title' => array (
        'name' => t('Title'),
        'description' => t('Content title'),
      ),
      'description' => array (
        'name' => t('Description'),
        'description' => t('Content body'),
      ),
      'link' => array (
        'name' => t('link'),
        'description' => t('link'),
      ),
      'video' => array (
        'name' => t('video'),
        'description' => t('video url'),
      ),
       'author' => array (
        'name' => t('author'),
        'description' => t('author'),
      ),
    );
  }

}
